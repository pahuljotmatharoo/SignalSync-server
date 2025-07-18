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
#define MSG_SEND 1
#define MSG_LIST 2
#define MSG_EXIT 3

//we need a better way of doing join, even though it might not Seven be needed, as we are not returning any value from the connection

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

        recv_exact_username(new_user->username, 50, new_user->sockid);

        //this is the thread argument, need the current user as well as the list
        thread_arg *arg = malloc(sizeof(thread_arg));
        arg->curr = new_user;
        arg->list = client_list;

        insert_user(arg->list, arg->curr);

        //create the thread to run for the user
        pthread_create(&new_user->id, NULL, create_connection, arg);

        //list sending logic (split up into a function)
        user* a = client_list->head;
        while(a != NULL) {
            int type_of_message_list = MSG_LIST;
            send(a->sockid, &type_of_message_list, sizeof(type_of_message_list), 0);

            client_list_s* client_list_send = malloc(sizeof (client_list_s));
            client_list_send->size = client_list->size;

            user* temp_node = client_list->head;

            for(int i = 0; i < client_list->size; i++) {
                strcpy((client_list_send->arr[i]), (temp_node->username));
                temp_node = temp_node->next;
            }
            client_list_send->size = htonl(client_list_send->size);
            send(a->sockid, client_list_send, sizeof (client_list_s), 0);
            a = a->next;
    }
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