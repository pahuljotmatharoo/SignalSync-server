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
#include "chat_room_list.h"
#define MSG_SEND 1
#define MSG_LIST 2
#define MSG_EXIT 3
#define username_length 50

//need to add so that the user recieves all current groups when connecting first time

int main() {
    pthread_mutex_t mutex;
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

    //we're gonna need our chatroom list
    ChatRoomList* ChatRoom_list = malloc(sizeof(ChatRoomList));
    init_ChatRoom_list(ChatRoom_list);

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

        recv_exact_username(new_user->username, username_length, new_user->sockid);

        //this is the thread argument, need the current user as well as the list
        thread_arg *arg = malloc(sizeof(thread_arg));
        arg->curr = new_user;
        arg->list_of_users = client_list;
        arg->ChatRoom_list = ChatRoom_list;
        arg->mutex = &mutex;

        pthread_mutex_lock(&mutex);

        insert_user(client_list, new_user);

        //create the thread to run for the user
        pthread_create(&new_user->id, NULL, create_connection, arg);
        pthread_detach(new_user->id);
        send_list(client_list);

        pthread_mutex_unlock(&mutex);
}

    //destroy our created linked list
    destructor_user_list(client_list);
    //destroy out mutex
    pthread_mutex_destroy(&mutex);
    close(sock);
}