#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> 
#include <string.h>
#include "thread_functions.h"
#include "user_list.h"
#include "messages.h"

//we need a better way of doing join, even though it might not Seven be needed, as we are not returning any value from the connection
//need to add a semaphore for the linked list editing, as the threads can interrupt each other, or read unpdated values

int main() {
    //create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //create our server's information
    struct sockaddr_in server;
    server.sin_family = AF_INET; //specify to use IPV4
    server.sin_port = htons(2520); // port is 2520
    server.sin_addr.s_addr = htonl(INADDR_ANY); // this is our ipv4 address

    //bind socket to the port and server info
    bind(sock, (struct sockaddr*) &server, sizeof(server));

    //waiting for a connection
    listen(sock, 25);

    //we're gonna need our client list
    user_list *client_list = malloc(sizeof(user_list));
    init_user_list(client_list);

    while (1) {
        //this is storing the clients information
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int new_sock = accept(sock, (struct sockaddr*)&client, &client_len);

        if (new_sock < 0) {
            perror("accept");
            continue;
        }

        //set up the new user (should get cleaned by the destructor)
        user *new_user = malloc(sizeof(user));
        pthread_t id;
        new_user->client = client;
        new_user->next = NULL;
        new_user->sockid = new_sock;
        new_user -> id = id;

        //print the list of current clients connected
        //print_client_list(client_list);

        //this is the thread argument, need the current user as well as the list
        thread_arg *arg = malloc(sizeof(thread_arg));
        arg->curr = new_user;
        arg->list = client_list;

        //create the thread to run for the user
        pthread_create(&new_user->id, NULL, create_connection, arg);
    }

    //this is temporary, need a better solution
    user* temp = client_list -> head;
    while(temp != NULL) {
        pthread_join(temp->id, NULL);
        temp = temp->next;
    }

    //destroy our created linked list
    destructor_user_list(client_list);
    close(sock);
}