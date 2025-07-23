#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

#include "user.h"
#include "user_list.h"

typedef struct thread_arg {
    user      *curr;
    user_list *list_of_users;
    pthread_mutex_t* mutex;
} thread_arg;

size_t recv_exact_msg(void* buf, size_t len, int sock);
void recv_exact_username(char* temp, size_t len, int sock);
void *create_connection(void *arg);
void send_list(user_list* client_list);


#endif /* THREAD_FUNCTIONS_H */
