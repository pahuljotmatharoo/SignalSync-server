#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

#include "user.h"
#include "user_list.h"

typedef struct thread_arg {
    user      *curr;
    user_list *list;
} thread_arg;

void *create_connection(void *arg);

#endif /* THREAD_FUNCTIONS_H */
