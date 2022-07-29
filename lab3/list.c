#include "list.h"
#include <string.h>
#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <pthread.h>

void insertElement ( int obj , struct head * self)
{
    struct element* p = ( struct element *) malloc ( sizeof ( struct element ) ) ;
    p->number = obj;
    p ->previous = NULL;
    pthread_mutex_lock(&(self->lock));
    p ->next = self -> next;
    if(self -> next != NULL){
        self ->next -> previous = p; 
    }
    self ->next = p;
    self->length += 1;
    pthread_mutex_unlock(&(self->lock));
}

void removeElement( int obj , struct head * self ) // first ptr == begin->next
{
    pthread_mutex_lock(&(self->lock));
    struct element * ptr = self->next;
    while( ptr != NULL){
        if(ptr->number == obj){
            struct element * pre = ptr->previous;
            struct element * nxt = ptr->next;
            if(pre == NULL){
                self->next= nxt;
            }
            else{
                pre->next = nxt;
            }
            if(nxt != NULL){
                nxt->previous = pre;
            }
            self->length--;
            break;
        }
        ptr = ptr->next;
    }
    pthread_mutex_unlock(&(self->lock));
    free(ptr);
}

void printElements(struct head * self ){
    pthread_mutex_lock(&(self->lock));
    struct element * ptr = self->next;
    while( ptr != NULL){
        printf("%d -> ", ptr->number);
        ptr = ptr->next;
    }
    printf("NULL");
    pthread_mutex_unlock(&(self->lock));
}

int popOfList(struct head * self ){
    pthread_mutex_lock(&(self->lock));
    if (self->length == 0){
        pthread_mutex_unlock(&(self->lock));
        return -1;
    }
    int x = self->next->number;
    struct element * ptr = self->next;
    struct element * pre = ptr->previous;
    struct element * nxt = ptr->next;
    if(pre == NULL){
        self->next= nxt;
    }
    else{
        pre->next = nxt;
    }
    if(nxt != NULL){
        nxt->previous = pre;
    }
    self->length -= 1;
    pthread_mutex_unlock(&(self->lock));
    free(ptr);
    return x;
}

struct head * list_init(){
    struct head * begin = ( struct head*) malloc ( sizeof ( struct head ) ) ;
    if(pthread_mutex_init(&(begin->lock),NULL) != 0){
        printf("\n mutex init failed");
        free(begin);
        return begin;
    }
    else{
        begin->next = NULL;
        begin->insert = &insertElement;
        if(begin->insert == NULL){
            printf("list_iafewrgernit");
        }
        begin->remove = &removeElement;
        begin->print = &printElements;
        begin->pop = &popOfList;
        begin->length = 0;
        return begin;
    }
}

void list_delete(struct head * begin){
    pthread_mutex_destroy(&(begin->lock));
    struct element* ptr = begin->next;
    while(ptr != NULL){
        struct element* to_delete = ptr;
        ptr = ptr->next;
        free(to_delete);
    }
    free(begin);
}

