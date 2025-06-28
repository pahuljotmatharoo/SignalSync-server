#ifndef USER_H
#define USER_H

#include <pthread.h>
#include <netinet/in.h>   /* for struct sockaddr_in */

typedef struct user {
    pthread_t         id;
    struct sockaddr_in client;
    struct user      *next;
    int               sockid;
} user;

#endif /* USER_H */
