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
    char arr[1024];
    int curr_pointer;
} message_r;

//message to send to the user
typedef struct message_to_send {
    int socketid;
    char *arr;
    int *data_size;
} message_s;

//reset our recieve array
void clear_message_r_arr(message_r *a) {
    memset(a->arr, 0, 1024);
    a->curr_pointer = 0;
    return;
}

void print_data(message_r *a) {
    for (int i = 0; i < a->curr_pointer; i++) {
        putchar(a->arr[i]);
        putchar(' ');
    }
}