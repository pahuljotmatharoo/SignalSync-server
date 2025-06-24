#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> 
#include <string.h>

//so need to make it so that the recieve array keeps getting filled instead of over-writing
//error in send to client

typedef struct message_to_recieve {
    int socketid;
    char arr[1024];
    int curr_pointer;
} message_r;

typedef struct message_to_send {
    int socketid;
    char *arr;
    int *data_size;
} message_s;

//we should send in the new socket id and the thing that we want to send 
void* send_to_client(void *arg) {
    
    message_s* message_to_send = (message_s*) arg;

    int sent = send(message_to_send->socketid, message_to_send->arr, *(message_to_send->data_size), 0);

    int *ret = malloc(sizeof *ret);
    *ret = sent;

    pthread_exit(ret);
}

//we should send in the new socket id and the thing that we want to send 
void* recieve_from_client(void *arg) {
    //lets just say that max message size is 128 characters.

    char temp[128];

    message_r* message_to_recieve = (message_r*) arg;
    int recieve = recv(message_to_recieve->socketid, temp, sizeof(message_to_recieve->arr), 0);

    temp[recieve] = '\0';
    
    for(int i = 0; i < recieve; i++) {
        message_to_recieve->arr[message_to_recieve->curr_pointer+i] = temp[i];
    }

    message_to_recieve->curr_pointer += recieve;
    message_to_recieve->arr[message_to_recieve->curr_pointer] = '\0';

    int *ret = malloc(sizeof *ret);
    *ret = recieve;

    pthread_exit(ret);
}


int main(int argc, char **argv) {
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
    int lis = listen(sock, 5);

    while (1) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int new_sock = accept(sock, (struct sockaddr*)&client, &client_len);

        if (new_sock < 0) {
            perror("accept");
            continue;
        }

        printf("Connection Established!\n");

        //so we're gonna have to make a growable array here
        char buf[32];

        char data[1024];
        memset(data, 0, sizeof data);
        char* msg = "Hello!";
        int *size = malloc(sizeof(int));
        *size = sizeof(msg);

        //this should get free'd by the thread
        message_s *sent = malloc(sizeof(message_s));
        sent->socketid = new_sock;
        sent->arr = msg;
        sent->data_size = size;

        //this should get free'd by the thread
        message_r *rec = malloc(sizeof(message_r));
        rec->socketid = new_sock;
        rec->curr_pointer = 0;

        int n; 
        while((n = recv(new_sock, buf, sizeof(buf) - 1, 0)) > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            if(strcmp(buf, "Sending") == 0) {
                pthread_t tid;
                void* bytes;

                pthread_create(&tid, NULL, recieve_from_client, sent);
                pthread_join(tid, &bytes);

                int *bytes_recieved = (int*) bytes;
                printf("Received from the send: %s\n", rec->arr);
                free(bytes);
                bytes_recieved = NULL;
            }
            else if(strcmp(buf, "Recieving") == 0) {
                pthread_t tid;
                void* bytes;

                pthread_create(&tid, NULL, send_to_client, rec);
                pthread_join(tid, &bytes);

                int *bytes_sent = (int*) bytes;
                if (*bytes_sent < 0) 
                    perror("send");

                printf("Sent to the client %d\n", *bytes_sent);
                free(bytes);
                bytes_sent = NULL;
            }
            else if(strcmp(buf, "Exiting") == 0) {
                printf("Closing Connection. \n");
                memset(buf, 0, sizeof buf);
                break;
            }
            memset(buf, 0, sizeof buf);
        }
        close(new_sock);
        free(rec);
        free(sent);
        free(size);
    }
    close(sock);
}