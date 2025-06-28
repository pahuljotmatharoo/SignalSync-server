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
    int socketid;
    char *arr;
    int *data_size;
} message_s;

typedef struct message_recieved {
    char arr[128];
    int ip;
} recieved_message;


#endif /* MESSAGES_H */