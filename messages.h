#ifndef MESSAGES_H
#define MESSAGES_H

//message to recieve from the user 
typedef struct message_to_recieve {
    int socketid;
    char *arr;
    int ip;
} message_r;

//message to send to the user
typedef struct message_to_send {
    char arr[128];
    int type;
} message_s;

typedef struct message_recieved {
    char arr[128];
    int ip;
} recieved_message;

typedef struct list {
    int size;
    int arr[10];
} client_list;


#endif /* MESSAGES_H */