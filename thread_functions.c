#include "data_structures.h"

//prolly don't want seperate thread functions for recieve and send tbh, seems useless
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

    //if we are at the end of the array, clear the data and rest the pointer
    if(message_to_recieve->curr_pointer >= 1024) {
        clear_message_r_arr(message_to_recieve);
    }
    
    //copy to the real array
    for(int i = 0; i < recieve; i++) {
        message_to_recieve->arr[message_to_recieve->curr_pointer+i] = temp[i];
    }

    message_to_recieve->curr_pointer += recieve;
    message_to_recieve->arr[message_to_recieve->curr_pointer] = '\0';

    print_data(message_to_recieve);

    int *ret = malloc(sizeof *ret);
    *ret = recieve;

    pthread_exit(ret);
}

void *create_connection(void *arg) {
        user* curr_user = (user*)arg;
        printf("Connection Established!\n");

        //so we're gonna have to make a growable array here
        char buf[32];

        char* msg = "Hello!";
        int *size = malloc(sizeof(int));
        *size = sizeof(msg);

        //this should get free'd by the thread (this is the message to send to the client)
        message_s *sent = malloc(sizeof(message_s));
        sent->socketid = curr_user->sockid;
        sent->arr = msg;
        sent->data_size = size;

        //this should get free'd by the thread
        message_r *rec = malloc(sizeof(message_r));
        rec->socketid = curr_user->sockid;
        rec->curr_pointer = 0;

        int n; 
        while((n = recv(curr_user->sockid, buf, sizeof(buf) - 1, 0)) > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            //the client is sending us information
            if(strcmp(buf, "Sending") == 0) {
                pthread_t tid;
                void* bytes;

                pthread_create(&tid, NULL, recieve_from_client, rec);
                pthread_join(tid, &bytes);

                int *bytes_recieved = (int*) bytes;
                printf("Bytes received from the send: %d\n", *bytes_recieved);
                free(bytes);
                bytes_recieved = NULL;
            }

            //we are sending information to the client here 
            else if(strcmp(buf, "Recieving") == 0) {
                pthread_t tid;
                void* bytes;

                pthread_create(&tid, NULL, send_to_client, sent);
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
        close(curr_user->sockid);
        free(rec);
        free(sent);
        free(size);
        pthread_exit(NULL);
}
