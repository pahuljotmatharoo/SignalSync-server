#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* for strcmp, strncpy */
#include <unistd.h>     /* for close() */
#include "user_list.h"
#include "thread_functions.h"
#include "messages.h"
#define MSG_SEND 1
#define MSG_LIST 2
#define MSG_EXIT 3

size_t recv_exact_msg(void* buf, size_t len, int sock) {
	recieved_message* temp = (recieved_message*)buf;
	size_t total = 0;
	while (total < len) {
		size_t r = recv(sock, (temp) + total, len - total, 0);
		if (r == 0)  return 0;   // peer closed
		total += r;
	}
	return total;
}

void recv_exact_username(char* temp, size_t len, int sock) {
    size_t total = 0;
    while(total < len) {
        size_t r = recv(sock, temp+total, len - total, 0);
        if (r == 0)  return; 
        total += r;
    }
}

void *create_connection(void *arg) {
    //here we can initalize 
        int n; 
        char temp[50];
        MsgHeader hdr;

        message_s *message_to_send = (message_s*) malloc(sizeof(message_s));

        thread_arg* curr_user = (thread_arg*)arg;
        int current_user_socket = curr_user->curr->sockid;

        //semaphore here, so that inserting the user does not conflict between threads
        sem_wait(&curr_user->list->sem); {

        insert_user(curr_user->list, curr_user->curr);

        }sem_post(&curr_user->list->sem);

        print_client_list(curr_user->list);

        printf("Connection Established!\n");
        printf("IP: %d \n", curr_user->curr->client.sin_addr.s_addr);

        recv_exact_username(&temp, 50, curr_user->curr->sockid);

        strcpy(&(curr_user->curr->username),temp);

        //so here, it will recieve, then execute based on the recieve, then continue again
        while((n = recv(current_user_socket, &hdr, sizeof(hdr), 0)) > 0) {

            uint32_t type   = ntohl(hdr.type);
            uint32_t length = ntohl(hdr.length);

            printf("Received: %d\n", type);

            //the client is sending us information
            if(type == MSG_SEND) {

                recieved_message a;

                //int recieve = recv(curr_user->curr->sockid, &a, sizeof(a), MSG_WAITALL);
                size_t recieve =recv_exact_msg(&a, 178, current_user_socket);
                                    
                //copy to the real array (its replacing the whole array)
                strncpy(message_to_send->arr, a.arr, 128);
                print_data(message_to_send);
                printf("\n");
                printf("Bytes received from the send: %d\n", recieve);
                        
                //so now that we have recieved the thing to send to a specific ip, we're gonna find corresponding socket
                user* temp = curr_user->list->head;

                while(temp != NULL && strcmp(a.user_to_send, temp->username) != 0) {
                    temp = temp->next;
                }
                        
                if(temp == NULL) {
                    continue;
                }

                //this is the type, letting the client know we are sending a message
                int type_of_message = MSG_SEND;
                send(temp->sockid, &type_of_message, sizeof(type_of_message), 0);

                int sent = send(temp->sockid, message_to_send, 128, 0);

                printf("Sent to the new client: %d\n", sent);
            }
            else if(type == MSG_LIST) {

                int type_of_message_list = MSG_LIST;
                send(current_user_socket, &type_of_message_list, sizeof(type_of_message_list), 0);

                client_list_s* client_list_send = malloc(sizeof (client_list_s));
                client_list_send->size = curr_user->list->size;

                user* temp_node = curr_user->list->head;

                for(int i = 0; i < curr_user->list->size; i++) {
                    strcpy(&(client_list_send->arr[i]), &(temp_node->username));
                    temp_node = temp_node->next;
                }

                // for(int i = curr_user->list->size; i < 10; i++) {
                //     strcpy(&(client_list_send->arr[i]), '\0');
                // }

               send(current_user_socket, client_list_send, sizeof (client_list_s), 0);
            }
            else if(type == MSG_EXIT) {
                printf("Closing Connection. \n");
                break;
            }
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