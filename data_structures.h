#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> 
#include <string.h>
//need a data structure to store the diff addresses of users, linkedlist prolly the best way to do this...

//define this early so we can use it...
typedef struct user user;

//LinkedList class
typedef struct user_list {
    user* head;
    user* tail;
    int size;
} user_list;

//Node class
struct user {
    pthread_t id;
    struct sockaddr_in  client;
    user* next;
    int sockid;
};

//message to recieve from the user 
typedef struct message_to_recieve {
    int socketid;
    char *arr;
    int ip;
} message_r;

//message to send to the user
typedef struct message_to_send {
    int socketid;
    char *arr;
    int *data_size;
} message_s;

typedef struct message_recieved {
    char arr[128];
    int ip;
} recieved_message;

typedef struct thread_arg {
    user* curr;
    user_list* list;
} thread_arg;

void print_data(message_r *a) {
    for (int i = 0; i < 128; i++) {
        putchar(a->arr[i]);
        putchar(' ');
    }
}