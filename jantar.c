// IMPORTACOES //

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <pthread.h>    /* POSIX Threads */
#include <semaphore.h>  /* Semaphore */


// DEFINES //
#define NUMTHREAD 5
#define N 5 /* numero de filosofos */
#define LEFT (i+N−1)%N /* numero do vizinho a esquerda de i */
#define RIGHT (i+1)%N /* numero do vizinho a direita de i */
#define THINKING 0 /* o filosofo esta pensando */
#define HUNGRY 1 /* o filosofo esta tentando pegar garfos */
#define EATING 2 /* o filosofo esta comendo */

// METODOS DOS FILOSOFOS //
void philosopher(int i) /* i: o numero do filosofo, de 0 a N–1 */
{
    while (TRUE) { /* repete para sempre */
        think( ); /* o filosofo esta pensando */
        take_forks(i); /* pega dois garfos ou bloqueia */
        eat( ); /* hummm, espaguete! */
        put_forks(i); /* devolve os dois garfos a mesa */
    }
}

void take_forks(int i) /* i: o numero do filosofo, de 0 a N–1 */
{
    down(&mutex); /* entra na regiao critica */
    state[i] = HUNGRY; /* registra que o filosofo esta faminto */
    test(i); /* tenta pegar dois garfos */
    up(&mutex); /* sai da regiao critica */
    down(&s[i]); /* bloqueia se os garfos nao foram pegos */
}

void put_forks(i) /* i: o numero do filosofo, de 0 a N–1 */
{
    down(&mutex); /* entra na regiao critica */
    state[i] = THINKING; /* o filosofo acabou de comer */
    test(LEFT); /* ve se o vizinho da esquerda pode comer agora */
    test(RIGHT); /* ve se o vizinho da direita pode comer agora */
    up(&mutex); /* sai da regiao critica */
}

void test(i)/* i: o numero do filosofo, de 0 a N–1 */
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
    state[i] = EATING;
    up(&s[i]);
    }   
}

// --------------------------------------------------------------------------------- //
//    SEMAFORO     //


/* prototype for thread routine */
void handler ( void *ptr );

/* semaforo declarado como global */
sem_t mutex;  // atua como um mutex
int counter;  // variavel compartilhada

int main()
{
    int id[NUMTHREAD];
    pthread_t thread[NUMTHREAD];
    
    // Identificadores para as Threads.
    for (int i=0; i < NUMTHREAD; i++) id[i] = i; 
    
    sem_init(&mutex, 0, 1);      /* initialize mutex to 1 - binary semaphore */
                                 /* second param = 0 - semaphore is local */
                                 
    /* criar as threads */
    for (int i=0; i < NUMTHREAD; i++) {
        pthread_create (&thread[i], NULL, (void *) &handler, (void *) &id[i]);
    }

    for (int i=0; i < NUMTHREAD; i++) {
        pthread_join(thread[i], NULL);
    }
                                

    sem_destroy(&mutex); /* destroy semaphore */
                  
    return 0;
} /* main() */

void handler ( void *ptr )
{
    int x; 
    x = *((int *) ptr);
    printf("Thread %d: Waiting to enter critical region...\n", x);
    sem_wait(&mutex);       /* down semaphore */
    /* START CRITICAL REGION */
    printf("Thread %d: Now in critical region...\n", x);
    printf("Thread %d: Counter Value: %d\n", x, counter);
    printf("Thread %d: Incrementing Counter...\n", x);
    counter++;
    printf("Thread %d: New Counter Value: %d\n", x, counter);
    printf("Thread %d: Exiting critical region...\n", x);
    /* END CRITICAL REGION */    
    sem_post(&mutex);       /* up semaphore */
    
    pthread_exit(0); /* exit thread */
}