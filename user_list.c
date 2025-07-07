#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "user_list.h"

void init_user_list(user_list *a) {
    a->head = NULL;
    a->tail = NULL;
    pthread_mutex_init(&a->mutex, NULL);
}

void destructor_user_list(user_list *ulist) {
    user *cur = ulist->head;
    while (cur) {
        user *next = cur->next;
        free(cur);             // free the user struct
        cur = next;
    }
    ulist->head = NULL;

    //destroy the mutex at the end
    pthread_mutex_destroy(&(ulist->mutex));
    free(ulist);
}

void insert_user(user_list *a, user* client) {
    //this means that its the first node
    if(a->head == NULL) {
        a->head = client;
        a->tail = client;
    }
    //we insert to the end always
    else {
        a->tail->next = client;
        a->tail = client;
    }
    a->size++;
    return;
}

//just removes the client from the list of current clients when they disconnect
void remove_user(user_list *a, user* client) {
    user* temp = a->head;
    if(temp == client) {
        a->head = a->head->next;
        if(a->tail == temp) {
            a->tail = a->head;
        }
        free(temp);
        a->size--;
        return;
    }
    while(temp->next != NULL) {
        if(temp->next == client) {
            user* temp_new = temp->next->next;
            free(temp->next);
            temp->next = temp_new;
            a->size--;
            return;
        }
        temp = temp->next;
    }
}

void print_client_list(user_list *a) {
    user* temp = a->head;
    while(temp != NULL) {
        printf("%d ", temp->client.sin_addr.s_addr);
        temp = temp -> next;
    }
}