#include "list.h"
#include <string.h>
#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


struct thread_pool
{
    pthread_t* t;
    pthread_t* this;
    int n;
    struct head* input;
    struct head* output;
    //void (*function) (struct head* input,struct head* output);
    int (*function) (void *i);
};

struct arg_f{
    struct thread_pool* t_p;
    int index_of_thread;
};

void * f ( void * i )
{
    struct arg_f *args = (struct arg_f *)i;
    struct thread_pool* t_p = args->t_p;
    int index = args->index_of_thread;
    free(args);
    while(true){
        int x =t_p->input->pop(t_p->input);
        //printf("%d %d\n",index, x);
        if( x < 1){
            break;
        }
        
        t_p->output->insert(t_p->function((void *)&x),t_p->output);
        
    }
    //printf("taaak \n");
    //pthread_exit(NULL);
    return NULL ;
}

struct thread_pool * create_thread_pool(int number_of_threads, struct head* input,struct head* output,void* x){
    struct thread_pool * obj = ( struct thread_pool*) malloc ( sizeof ( struct thread_pool ) ) ;
    obj->n = number_of_threads;
    obj->t = ( pthread_t *) malloc ( obj->n*sizeof ( pthread_t) ) ;
    obj->input = input;
    obj->output = output;
    obj->function = (int (*) (void *))x;
    obj->this = ( pthread_t *) malloc (sizeof ( pthread_t) ) ;
    return obj;
}



void * function_to_pool ( void * i )
{
    struct thread_pool *t_p = (struct thread_pool *)i;
    for(int j = 0 ; j < t_p->n ; j++){
        pthread_create (&t_p->t[j] , NULL , f , i ) ;
    }
    for(int j = 0 ; j < t_p->n ; j++){
        pthread_join (t_p->t[j] , NULL);
    }
    return NULL ;
}


int ourTask (void * j){
    int* x = (int* )j;
    if (*x>1){
        int i = 2;
        while(i<*x){
            if(*x%i == 0){
                break;
            }
            i++;
        }
        if(*x == i){
            return 1;
        }
    }
    return 0;
}

void pool_start(struct thread_pool * pool){
    //pthread_create ( pool->this , NULL , function_to_pool, (void *)pool ) ;
    for(int j = 0 ; j < pool->n ; j++){
        //printf("otwieram %d \n",j);
        struct arg_f* args =( struct arg_f *) malloc ( sizeof (struct arg_f));
        args->t_p = pool;
        args->index_of_thread = j;
        pthread_create (&pool->t[j] , NULL , f , (void *)args ) ;
        //printf("otwarte %d \n",j);
    }

    for(int i = 0 ; i < pool->n ; i++){
        //printf("zamykam %d \n",i);
        pthread_join (pool->t[i] , NULL);
        //printf("zamkniete %d \n",i);
    }
}

void delete_thread_pool(struct thread_pool * pool){
    free(pool->t);
    free(pool->this);
    free(pool);
}

void main ( void )
{
    int n = 3;
    int number= 10000;
    struct head* sourse  = list_init();
    for(int i = 1; i <= number; i++){
        sourse->insert(i,sourse);
    }
    //printf("\n%d\n", sourse->length);
    //sourse->print(sourse);
    struct head* found  = list_init();
    struct thread_pool * t_p = create_thread_pool(n,sourse,found,(void *)&ourTask);
    pool_start(t_p);
    //pool_join(t_p);
    delete_thread_pool(t_p);
    //found->print(found);
    //printf("\n");
    int x = 0;
    for(int i = 0; i<number;i++){
        if(found->pop(found) == 1){
            x++;
        }
    }

    printf("\n%d\n", x);
    list_delete(sourse);
    list_delete(found);
}