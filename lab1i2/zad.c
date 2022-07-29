#include <string.h>
#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <pthread.h>


int liczbaPetli = 1000000;

struct element
{
    struct element* previous ;
    int number;
    struct element* next ;
};

struct head{
    struct element* next ;
    void (*insert) ( int obj , struct head * self  );
    void  (*remove)( int obj , struct head * self );
    int (*length)(struct head * self );
    void (*print)(struct head * self );
    pthread_mutex_t lock;
} *begin;

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
            break;
        }
        ptr = ptr->next;
    }
    pthread_mutex_unlock(&(self->lock));
}

void printElements(struct head * self ){
    struct element * ptr = self->next;
    int x = 0;
    while( ptr != NULL){
        printf("%d -> ", ptr->number);
        ptr = ptr->next;
    }
    printf("NULL");
}

int LengthOfList(struct head * self ){
    struct element * ptr = self->next;
    int x = 0;
    while( ptr != NULL){
        x++;
        ptr = ptr->next;
    }
    return x;
}

int list_init(){
    begin = ( struct head*) malloc ( sizeof ( struct head ) ) ;
    if(pthread_mutex_init(&(begin->lock),NULL) != 0){
        printf("\n mutex init failed");
        free(begin);
        return 1;
    }
    else{
        begin->next = NULL;
        begin->insert = &insertElement;
        if(begin->insert == NULL){
            printf("list_iafewrgernit");
        }
        begin->remove = &removeElement;
        begin->length = &LengthOfList;
        begin->print = &printElements;
        return 0;
    }
}

void list_delete(){
    struct element* ptr = begin->next;
    while(ptr != NULL){
        struct element* to_delete = ptr;
        ptr = ptr->next;
        free(to_delete);
    }
    free(begin);
}

void * f ( void * i )
{
    for(int j = 0 ; j < liczbaPetli; j++){
        begin->insert(j, begin) ;
    }
    return NULL ;
}

void main ( void )
{
    list_init();
    /*
    for( int i = 0 ; i < 20 ; i++){
        begin ->insert(i, begin ) ;
    }
    begin->print(begin);
    printf("\n");
    begin ->remove(5, begin ) ;
    begin -> remove(9, begin ) ;
    begin -> remove(10 , begin ) ;
    begin->print(begin);
    printf("\n");
    begin ->insert(120, begin ) ;
    begin->print(begin);
    printf("\n");
    */
    int n = 3;
    pthread_t* watki = ( pthread_t *) malloc ( n*sizeof ( pthread_t) ) ;
    
    for(int i = 0 ; i < n ; i++){
            pthread_create (& watki[i] , NULL , f , NULL ) ;
    }
    for(int i = 0 ; i < n ; i++){
            pthread_join (watki[i] , NULL) ;
    }
    free(watki);
    int liczbaUdanychProb = begin->length(begin);
    double procent = (double)(liczbaUdanychProb) / (double)(n) /liczbaPetli* 100;
    printf("Liczba Elementow: %i\nProcent: %f %%",liczbaUdanychProb,procent);
    
    list_delete();
        
}
