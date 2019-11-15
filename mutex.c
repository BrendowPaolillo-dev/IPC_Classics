#include<stdio.h>
#include <stdlib.h> 
#include <time.h>  
#include<pthread.h> 
#include<unistd.h> 

#define size (200)
#define N 3      //numero de threads leitoras
#define M 1       //numero de threads escritoras

pthread_t t[N];          // define N
pthread_mutex_t mutex;   // variável mutex
pthread_mutex_t db;      // variável db
int counter = 0;

int write_data_file(){
  int value;
  FILE *f;
  f = fopen("data.txt", "w");

  value = rand() % 10;

  if(f == NULL){
    printf("erro na leitura do arquivo");
    //return -1;
  }else{
    fprintf(f, "%d", value);
    //fflush(stdin);
    printf("Valor escrito: %d\n", value);
    fflush(stdout);
  }
  fclose(f);

  return value;
}

int read_data_file(){
  int value;
  FILE *f;
  f = fopen("data.txt", "r");
  if(f == NULL){
    printf("erro na leitura do arquivo");
    //return -1;
  }else
    fscanf(f, "%d", &value);
  
  fclose(f);

  return value;
}


void* reader(void *arg){
  int value;
  pthread_mutex_lock(&mutex);  
  counter = counter + 1;
  if (counter == 1)
    pthread_mutex_lock(&db);
  pthread_mutex_unlock(&mutex);
  value = read_data_file();
  pthread_mutex_lock(&mutex);
  counter = counter - 1;
  if(counter == 0)
    pthread_mutex_unlock(&db);
  pthread_mutex_unlock(&mutex);
  
  printf("Valor lido: %d\n", value);
  fflush(stdout);
}

void* writer(void *arg){
  pthread_mutex_lock(&db);
  write_data_file();
  pthread_mutex_unlock(&db);
}


int main(void) 
{
  srand(time(0));
    int i, c_reader = 0, c_writer = 0;
    /* inicializa mutex - inicializado como LIVRE */ 
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&db, NULL);

    /* inicializa threads */
    for (i = 0; i < M+N; i++){
      if ((i == 0 || i%2 == 0) && c_writer < M){
        // printf("criando thread de escrita\n");
        pthread_create(&(t[i]), NULL, &writer, NULL); 
        c_writer ++;
    }else if (c_reader < N){
        // printf("criando thread de leitura\n");
        pthread_create(&(t[i]), NULL, &reader, NULL);
      }
    } 
        
  
    /* aguarda todas as threads finalizarem */
    for (i = 0; i < M+N; i++){
        printf("executando a thread %d\n", i);
        fflush(stdout);
        pthread_join(t[i], NULL); 
    }
    /* destroi mutex */
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&db); 
  
    return 0; 
} 