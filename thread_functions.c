#include <pthread.h>
#include <stdio.h>
#include <string.h>     /* for strcmp, strncpy */
#include <unistd.h>     /* for close() */
#include "user_list.h"
#include "thread_functions.h"
#include "messages.h"

void *create_connection(void *arg) {
        thread_arg* curr_user = (thread_arg*)arg;

        //semaphore here, so that inserting the user does not conflict between threads
        sem_wait(&curr_user->list->sem); {

        insert_user(curr_user->list, curr_user->curr);

        }sem_post(&curr_user->list->sem);

        print_client_list(curr_user->list);

        printf("Connection Established!\n");
        printf("IP: %d \n", curr_user->curr->client.sin_addr.s_addr);

        //temp buffer
        char buf[32];

        message_s *message_to_send = malloc(sizeof(message_s));

        int n; 
        while((n = recv(curr_user->curr->sockid, buf, sizeof(buf) - 1, 0)) > 0) {
            
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            //the client is sending us information
            if(strcmp(buf, "Sending") == 0) {

                recieved_message a;

                int recieve = recv(curr_user->curr->sockid, &a, sizeof(a), MSG_WAITALL);
                             
                //copy to the real array (its replacing the whole array)
                strncpy(message_to_send->arr, a.arr, 128);
                print_data(message_to_send);
                printf("\n");
                printf("Bytes received from the send: %d\n", recieve);

                //this is the type, letting the client know we are sending a message
                int type_of_message = 0;
                int x =send(curr_user->curr->sockid, &type_of_message, sizeof(type_of_message), 0);
                
                //so now that we have recieved the thing to send to a specific ip, we're gonna find corresponding socket
                user* temp = curr_user->list->head;

                while((int)(temp ->client.sin_addr.s_addr) != a.ip && temp != NULL) {
                    temp = temp->next;
                }
                
                if(temp == NULL) {
                    continue;
                }

                int sent = send(temp->sockid, message_to_send, 128, 0);

                printf("Sent to the new client: %d\n", sent);
            }
            else if(strcmp(buf, "List") == 0) {

                int type_of_message_list = 1;
                send(curr_user->curr->sockid, &type_of_message_list, sizeof(type_of_message_list), 0);

                client_list_s* client_list_send = malloc(sizeof (client_list_s));
                client_list_send->size = curr_user->list->size;

                user* temp_node = curr_user->list->head;

                for(int i = 0; i < curr_user->list->size; i++) {
                    client_list_send->arr[i] = temp_node->client.sin_addr.s_addr;
                    temp_node = temp_node->next;
                }

                for(int i = curr_user->list->size; i < 10; i++) {
                    client_list_send->arr[i] = 0;
                }

                int sent = send(curr_user->curr->sockid, client_list_send, sizeof (client_list_s), 0);
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

        //semaphore here
        sem_wait(&curr_user->list->sem); {
        remove_user((curr_user->list), (curr_user->curr));
        }sem_post(&curr_user->list->sem);

        print_client_list(curr_user->list);

        free(arg);
        
        pthread_exit(NULL);
}