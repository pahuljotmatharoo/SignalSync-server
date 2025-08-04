#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "user_list.h"
#include "thread_functions.h"
#include "messages.h"
#define MSG_SEND 1
#define MSG_LIST 2
#define MSG_EXIT 3
#define USER_EXIT 4
#define ROOM_CREATE 5
#define ROOM_MSG 6
#define username_length 50
#define message_length 128

//still need to add that whenever a new group is added, all other threads are notified

//every user has a thread on the server, so when they send to the server,
//their own thread is processing the information and sending to other users/all users for grp

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

void send_list(user_list* client_list) {
    user* temp = client_list->head;
    while(temp != NULL) {
            int type_of_message_list = MSG_LIST;
            send(temp->sockid, &type_of_message_list, sizeof(type_of_message_list), 0);

            client_list_s* client_list_send = malloc(sizeof (client_list_s));
            client_list_send->size = client_list->size;

            user* temp_node = client_list->head;

            for(int i = 0; i < client_list->size; i++) {
                strcpy((client_list_send->arr[i]), (temp_node->username));
                temp_node = temp_node->next;
            }

            client_list_send->size = htonl(client_list_send->size);
            send(temp->sockid, client_list_send, sizeof (client_list_s), 0);
            temp = temp->next;
    }
}

void *create_connection(void *arg) {
    //here we can initalize 
        int n; 
        MsgHeader hdr;

        message_s *message_to_send = (message_s*) malloc(sizeof(message_s));
        message_s_group *message_to_send_group = (message_s_group*) malloc(sizeof(message_s_group));

        thread_arg* curr_user = (thread_arg*)arg;
        int current_user_socket = curr_user->curr->sockid;

        pthread_mutex_lock(curr_user->mutex);
        print_client_list(curr_user->list_of_users);
        pthread_mutex_unlock(curr_user->mutex);

        printf("Connection Established!\n");
        printf("IP: %d \n", curr_user->curr->client.sin_addr.s_addr);

        //so here, it will recieve, then execute based on the recieve, then continue again
        while((n = recv(current_user_socket, &hdr, sizeof(hdr), 0)) > 0) {

            uint32_t type   = ntohl(hdr.type);
            uint32_t length = ntohl(hdr.length);

            printf("Received: %d\n", type);

            //the client is sending us information
            if(type == MSG_SEND) {

                recieved_message a;

                //int recieve = recv(curr_user->curr->sockid, &a, sizeof(a), MSG_WAITALL);
                size_t recieve = recv_exact_msg(&a, sizeof(recieved_message), current_user_socket);
                                    
                //copy to the real array (its replacing the whole array)
                strncpy(message_to_send->arr, a.arr, message_length);
                strncpy(message_to_send->username, curr_user->curr->username, username_length);
                print_data(message_to_send);
                printf("\n");
                printf("Bytes received from the send: %d\n", (int)recieve);
                        
                //so now that we have recieved the thing to send to a specific ip, we're gonna find corresponding socket
                user* temp = curr_user->list_of_users->head;

                while(temp != NULL && strcmp(a.user_to_send, temp->username) != 0) {
                    temp = temp->next;
                }
                        
                if(temp == NULL) {
                    continue;
                }

                //this is the type, letting the client know we are sending a message
                int type_of_message = MSG_SEND;
                send(temp->sockid, &type_of_message, sizeof(type_of_message), 0);

                int sent = send(temp->sockid, message_to_send, sizeof(message_s), 0);

                printf("Sent to the new client: %d\n", sent);
            }
            else if(type == MSG_EXIT) {
                printf("Closing Connection. \n");
                break;
            }

            else if(type == ROOM_CREATE) {
                ChatRoom* newRoom = malloc(sizeof(ChatRoom));

                //since username is same size as chatroom name, we can simply use this function again
                recv_exact_username(newRoom->ChatRoomName, 50, current_user_socket);

                pthread_mutex_lock(curr_user->mutex);
                insert_ChatRoom(curr_user->ChatRoom_list, newRoom);
                pthread_mutex_unlock(curr_user->mutex);

                user* temp_u = curr_user->list_of_users->head;
                while(temp_u != NULL) {
                    if(strcmp(temp_u->username, curr_user->curr->username) != 0) {
                        int type_of_message = ROOM_CREATE;
                        send(temp_u->sockid, &type_of_message, sizeof(type_of_message), 0);

                        //this sends 50 characters
                        int sent = send(temp_u->sockid, newRoom->ChatRoomName, 50, 0);

                        printf("Sent to the new client: %d\n", sent);
                    }
                    temp_u = temp_u->next;
                }
            }

            else if(type == ROOM_MSG) {
                recieved_message a;

                //int recieve = recv(curr_user->curr->sockid, &a, sizeof(a), MSG_WAITALL);
                size_t recieve = recv_exact_msg(&a, sizeof(recieved_message), current_user_socket);

                //copy to the real array (its replacing the whole array)
                strncpy(message_to_send_group->arr, a.arr, message_length);
                strncpy(message_to_send_group->groupName, a.user_to_send, username_length);
                strncpy(message_to_send_group->username, curr_user->curr->username, username_length);
                //print_data(message_to_send);
                printf("\n");
                printf("Bytes received from the send: %d\n", (int)recieve);

                user* temp_u = curr_user->list_of_users->head;
                while(temp_u != NULL) {
                    if(strcmp(temp_u->username, curr_user->curr->username) != 0) {
                        int type_of_message = ROOM_MSG;
                        send(temp_u->sockid, &type_of_message, sizeof(type_of_message), 0);

                        int sent = send(temp_u->sockid, message_to_send_group, sizeof(message_s_group), 0);

                        printf("Sent to the new client: %d\n", sent);
                    }
                    temp_u = temp_u->next;
                }
            }
        }

        close(curr_user->curr->sockid);

        pthread_mutex_lock(curr_user->mutex);

        remove_user((curr_user->list_of_users), (curr_user->curr));

        user* temp = curr_user->list_of_users->head;
        
        while(temp != NULL) {
            int type_of_message_list = USER_EXIT;
            send(temp->sockid, &type_of_message_list, sizeof(type_of_message_list), 0);

            send(temp->sockid, curr_user->curr->username, sizeof(curr_user->curr->username), 0);

            temp = temp->next;
        }

        pthread_mutex_unlock(curr_user->mutex);

        free(arg);
        
        pthread_exit(NULL);
}