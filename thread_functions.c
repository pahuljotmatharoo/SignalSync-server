#include <pthread.h>
#include <stdio.h>
#include "thread_functions.h"
#include "messages.h"
#include <string.h>     /* for strcmp, strncpy */
#include <unistd.h>     /* for close() */
/* implement create_connection() */

void *create_connection(void *arg) {
        thread_arg* curr_user = (thread_arg*)arg;
        printf("Connection Established!\n");
        //printf("Thread ID: %d \n", curr_user->id);
        printf("IP: %d \n", curr_user->curr->client.sin_addr.s_addr);

        char buf[32];

        char* msg = "Hello!";
        int *size = malloc(sizeof(int));
        *size = sizeof(msg);

        //this should get free'd by the thread
        message_r *rec = malloc(sizeof(message_r));
        rec->socketid = curr_user->curr->sockid;
        rec->arr = malloc(128);

        int n; 
        while((n = recv(curr_user->curr->sockid, buf, sizeof(buf) - 1, 0)) > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            //the client is sending us information
            if(strcmp(buf, "Sending") == 0) {
                // we need to encode the destination with the message, best way seems to be a struct right

                recieved_message a;

                message_r* message_to_recieve = rec;
                int recieve = recv(message_to_recieve->socketid, &a, sizeof(a), 0);
                
                //copy to the real array (its replacing the whole array)
                strncpy(message_to_recieve->arr, a.arr, 128);
                message_to_recieve->ip = a.ip;
                print_data(message_to_recieve);

                printf("Bytes received from the send: %d\n", recieve);

                //so now that we have recieved the thing to send to a specific ip, we're gonna find it and send it to it
                int sending_sock;

                user* temp = curr_user->list->head;

                while((int)(temp ->client.sin_addr.s_addr) != a.ip && temp != NULL) {
                    temp = temp->next;
                }
                
                if(temp == NULL) {
                    continue;
                }
                
                sending_sock = temp->sockid;

                int sent = send(sending_sock, message_to_recieve->arr, 128, 0);

                printf("Sent to the new client: %d\n", sent);
            }
            else if(strcmp(buf, "Exiting") == 0) {
                printf("Closing Connection. \n");
                memset(buf, 0, sizeof buf);
                break;
            }
            memset(buf, 0, sizeof buf);
        }
        close(curr_user->curr->sockid);
        remove_user((curr_user->list), (curr_user->curr));
        free(rec);
        free(size);
        free(arg);
        pthread_exit(NULL);
}
