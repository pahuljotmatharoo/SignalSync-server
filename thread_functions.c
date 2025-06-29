#include <pthread.h>
#include <stdio.h>
#include "thread_functions.h"
#include "messages.h"
#include <string.h>     /* for strcmp, strncpy */
#include <unistd.h>     /* for close() */
#include "user_list.h"
/* implement create_connection() */

void *create_connection(void *arg) {
        thread_arg* curr_user = (thread_arg*)arg;
        printf("Connection Established!\n");
        //printf("Thread ID: %d \n", curr_user->id);
        printf("IP: %d \n", curr_user->curr->client.sin_addr.s_addr);

        char buf[32];

        //this should get free'd by the thread
        message_r *rec = malloc(sizeof(message_r));
        rec->socketid = curr_user->curr->sockid;
        rec->arr = malloc(128);

        message_s *message_to_send = malloc(sizeof(message_s));

        int n; 
        while((n = recv(curr_user->curr->sockid, buf, sizeof(buf) - 1, 0)) > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            //the client is sending us information
            if(strcmp(buf, "Sending") == 0) {
                // we need to encode the destination with the message, best way seems to be a struct right

                recieved_message a;

                int recieve = recv(curr_user->curr->sockid, &a, sizeof(a), 0);

                message_to_send->type = 0;
                
                //copy to the real array (its replacing the whole array)
                strncpy(message_to_send->arr, a.arr, 128);
                print_data(message_to_send);
                printf("\n");
                printf("Bytes received from the send: %d\n", recieve);

                int x = 1;
                send(curr_user->curr->sockid, &x, sizeof(x), 0);

                //so now that we have recieved the thing to send to a specific ip, we're gonna find it and send it to it

                user* temp = curr_user->list->head;

                while((int)(temp ->client.sin_addr.s_addr) != a.ip && temp != NULL) {
                    temp = temp->next;
                }
                
                if(temp == NULL) {
                    continue;
                }
                
                int sent = send(temp->sockid, message_to_send, 132, 0);

                printf("Sent to the new client: %d\n", sent);
            }
            else if(strcmp(buf, "List") == 0) {
                client_list* client_list_send = malloc(sizeof (client_list));
                client_list_send->size = curr_user->list->size;

                user* temp_node = curr_user->list->head;

                for(int i = 0; i < curr_user->list->size; i++) {
                    client_list_send->arr[i] = temp_node->client.sin_addr.s_addr;
                    temp_node = temp_node->next;
                }

                for(int i = curr_user->list->size; i < 10; i++) {
                    client_list_send->arr[i] = 0;
                }

                int sent = send(curr_user->curr->sockid, client_list_send, sizeof (client_list), 0);
                int x = 5;
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
        print_client_list(curr_user->list);
        free(rec);
        free(arg);
        pthread_exit(NULL);
}