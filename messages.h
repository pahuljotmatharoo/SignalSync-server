#ifndef MESSAGES_H
#define MESSAGES_H
#include <stdint.h>

//message to recieve from the user 
typedef struct message_to_recieve {
    int socketid;
    char *arr;
    int ip;
} message_r;

//message to send to the user
typedef struct message_to_send {
    char arr[128];
} message_s;

typedef struct message_recieved {
    char arr[128];
    char user_to_send[50];
} recieved_message;

typedef struct list {
    int size;
    char arr[10][50];
} client_list_s;

typedef struct Msg{
    uint32_t type;    // message type
    uint32_t length;  // payload length
} MsgHeader;

void print_data(message_s *a);

#endif /* MESSAGES_H */