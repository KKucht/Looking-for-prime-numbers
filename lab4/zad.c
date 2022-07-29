#include <string.h>
#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#define ODCZYT 0
#define ZAPIS 1


struct thread_pool
{
    int* pid;
    int n;
    int* input;
    int* output;
    int (*function) (void *i);
};

struct thread_pool * create_thread_pool(int number_of_threads, int* input,int* output,void* x){
    struct thread_pool * obj = ( struct thread_pool*) malloc ( sizeof ( struct thread_pool ) ) ;
    obj->n = number_of_threads;
    obj->pid = ( int *) malloc ( obj->n*sizeof ( int) ) ;
    obj->input = input;
    obj->output = output;
    obj->function = (int (*) (void *))x;
    return obj;
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

int liczba_elementow[2];


void pool_start(struct thread_pool * pool){
    for(int i = 0 ; i < pool->n ; i++){
        //printf("otwieram %d \n",i);
        pool->pid[i] = fork();
        if(pool->pid[i] == 0){
            close(pool->output[ODCZYT]);
            close(pool->input[ZAPIS]);
            while(true){
                int x = 0;
                int m = 0;
                read(liczba_elementow[ODCZYT], &m,sizeof(m));
                if(m == 0){
                    write(liczba_elementow[ZAPIS], &m,sizeof(m));
                    break;
                }
                read(pool->input[ODCZYT], &x,sizeof(x));
                m -= 1;
                write(liczba_elementow[ZAPIS], &m,sizeof(m));
                //printf("%d %d\n",i, x);
                x = pool->function((void *)&x);
                write(pool->output[ZAPIS], &x,sizeof(x));
            }
            //printf("taaak \n");
            close(liczba_elementow[ODCZYT]);
            close(liczba_elementow[ZAPIS]);
            close(pool->input[ODCZYT]);
            close(pool->output[ZAPIS]);
            exit(0);
        }
        //int x = -1;
        //write(pool->input[ZAPIS], &x, sizeof(x));
        //printf("otwarte %d \n",i);
    }

    for(int i = 0 ; i < pool->n ; i++){
        //printf("zamykam %d \n",i);
        waitpid(pool->pid[i],NULL,0);
        //printf("zamkniete %d \n",i);
    }
}

void delete_thread_pool(struct thread_pool * pool){
    free(pool->pid);
    free(pool);
}

void main ( void )
{
    int n = 3;
    int number= 10000;
    int input[2];
    int output[2];
    pipe(input);
    pipe(output);
    pipe(liczba_elementow);
    write(liczba_elementow[ZAPIS], &number, sizeof(number));
    for(int i = 1; i <= number; i++){
        write(input[ZAPIS], &i, sizeof(i));
    }

    struct thread_pool * t_p = create_thread_pool(n,input,output,(void *)&ourTask);
    pool_start(t_p);
    delete_thread_pool(t_p);

    int x = 0;
    for(int i = 0; i<number;i++){
        int k = 0;
        read(output[ODCZYT], &k,sizeof(k));
        if(k == 1){
            x++;
        }
    }

    printf("%d\n", x);
    close(liczba_elementow[ODCZYT]);
    close(liczba_elementow[ZAPIS]);
    close(input[ODCZYT]);
    close(input[ZAPIS]);
    close(output[ODCZYT]);
    close(output[ZAPIS]);

}