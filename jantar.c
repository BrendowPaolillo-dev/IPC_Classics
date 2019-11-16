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
#define LEFT ((i+(N - 1))%N) /* numero do vizinho a esquerda de i */
#define RIGHT (i+1)%N /* numero do vizinho a direita de i */
#define THINKING 0 /* o filosofo esta pensando */
#define HUNGRY 1 /* o filosofo esta tentando pegar garfos */
#define EATING 2 /* o filosofo esta comendo */
int state[N];
typedef int semaphore; /* semaforos sao um tipo especial de int */
int state[N]; /* arranjo para controlar o estado de cada um */
sem_t  mutex ; /* exclusao mutua para as regioes criticas */
sem_t  s[N]; /* um semaforo por filosofo */


/* prototypes */
void filosofo ( void *ptr );
void take_forks(int i);
void put_forks(int i);
void test(int i);
/* semaforo declarado como global */
// sem_t mutex;  // atua como um mutex
int counter;  // variavel compartilhada



void filosofo ( void *ptr )
{
    int x; 
    x = *((int *) ptr);
    //x = LEFT + 1;
    printf("\no filosofo %d esta pensando",x);

    take_forks(x);

    /* START CRITICAL REGION */
    printf("\ncomendo espaguete! ");
    /* END CRITICAL REGION */  

    put_forks(x); /* devolve os dois garfos a mesa */

    printf("terminou de comer");
    pthread_exit(0); /* exit thread */
}

void take_forks(int i) /* i: o numero do filosofo, de 0 a N–1 */
{
    sem_wait(&mutex); /* entra na regiao critica */
    state[i] = HUNGRY; /* registra que o filosofo esta faminto */
    test(i); /* tenta pegar dois garfos */
    sem_post(&mutex); /* sai da regiao critica */
    sem_wait(&s[i]); /* bloqueia se os garfos nao foram pegos */
}

void put_forks(int i) /* i: o numero do filosofo, de 0 a N–1 */
{
    sem_wait(&mutex); /* entra na regiao critica */
    state[i] = THINKING; /* o filosofo acabou de comer */
    test(LEFT); /* ve se o vizinho da esquerda pode comer agora */
    test(RIGHT); /* ve se o vizinho da direita pode comer agora */
    sem_post(&mutex); /* sai da regiao critica */
}

void test(int i)/* i: o numero do filosofo, de 0 a N–1 */
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
    state[i] = EATING;
    sem_post(&s[i]);
    }   
}

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
        pthread_create (&thread[i], NULL, (void *) &filosofo, (void *) &id[i]);
    }

    for (int i=0; i < NUMTHREAD; i++) {
        pthread_join(thread[i], NULL);
    }
                                

    sem_destroy(&mutex); /* destroy semaphore */
                  
    return 0;
} /* main() */

