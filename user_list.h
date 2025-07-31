#ifndef USER_LIST_H
#define USER_LIST_H
#include "user.h"
#include <pthread.h>

typedef struct user_list {
    user *head;
    user *tail;
    unsigned int   size;
} user_list;

void init_user_list(user_list *a);
void destructor_user_list(user_list *ulist);
void insert_user(user_list *a, user *client);
void remove_user(user_list *a, user *client);
void print_client_list(user_list *a);

#endif /* USER_LIST_H */
