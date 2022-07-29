#ifndef _list_h
#define _list_h
#include <pthread.h>
struct element
{
    struct element* previous ;
    int number;
    struct element* next ;
};

struct head{
    struct element* next ;
    int length;
    void (*insert) ( int obj , struct head * self  );
    void  (*remove)( int obj , struct head * self );
    void (*print)(struct head * self );
    int (*pop)(struct head * self );
    pthread_mutex_t lock;
};

void insertElement ( int obj , struct head * self);

void removeElement( int obj , struct head * self );

void printElements(struct head * self );


struct head *list_init();

void list_delete(struct head * begin);

int popOfList(struct head * self );

#endif