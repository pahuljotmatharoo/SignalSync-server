#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

#include "user.h"
#include "user_list.h"
#include "chat_room_list.h"
#include "messages.h"

typedef struct thread_arg {
    user      *curr;
    user_list *list_of_users;
    ChatRoomList* ChatRoom_list;
    pthread_mutex_t* mutex;
    pthread_mutex_t* user_fileMutex;
    pthread_mutex_t* group_fileMutex;
} thread_arg;

size_t recv_exact_msg(void* buf, size_t len, int sock);
void recv_exact_username(char* temp, size_t len, int sock);
void *create_connection(void *arg);
void send_list(user_list* client_list);
void send_chatroom_list(ChatRoomList* chatroom_list, int sockid);
void room_method_creation(user* temp, thread_arg* curr_user, int type_of_message, void* data, int size);
void room_method_message(recieved_message* a, user* temp, thread_arg* curr_user, int type_of_message, void* data, int size, thread_arg* threadArg);
void write_to_file_user(message_s * message_to_send_user, char* threadUsername, char* recvUsername, pthread_mutex_t *fileMutex);
void write_to_file_group(message_s_group * message_to_send_group, char* username, pthread_mutex_t *group_fileMutex);
void send_message_user(message_s *message_to_send, user* head, char username[50], int current_user_socket, thread_arg* threadArg);
void setupDir(char* username);
char* setupFileStringUser(char *username, char* username_to_send);
char* setupFileStringGroup(char* group);

#endif /* THREAD_FUNCTIONS_H */
