/*
  Produtor-Consumidor com mutex
  Autores: Brendow e Higor
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define size (200)
#define N 3 //numero de threads leitoras
#define M 2 //numero de threads escritoras

pthread_t t[N];        // define N
pthread_mutex_t mutex; // variável mutex
pthread_mutex_t db;    // variável db
int counter = 0;

int write_data_file()
{ //função que escreve um número aleatório no arquivo de texto
  int value;
  FILE *f;
  f = fopen("data.txt", "w");

  value = rand() % 10;

  if (f == NULL)
  {
    printf("erro na leitura do arquivo");
    //return -1;
  }
  else
  {
    fprintf(f, "%d", value);
    //fflush(stdin);
    printf("Valor escrito: %d\n", value);
    fflush(stdout);
  }
  fclose(f);

  return value;
}

int read_data_file()
{ //função que lê um número aleatório no arquivo de texto
  int value;
  FILE *f;
  f = fopen("data.txt", "r");
  if (f == NULL)
  {
    printf("erro na leitura do arquivo");
    //return -1;
  }
  else
    fscanf(f, "%d", &value);

  fclose(f);

  return value;
}

void *reader(void *arg)
{ //função das threads de leitura
  int value;
  pthread_mutex_lock(&mutex);   //ganha o acesso exclusivo do primeiro semáforo
  counter = counter + 1;        //conta quantidade de threads acessando
  if (counter == 1)             //se for a única thread
    pthread_mutex_lock(&db);    //ganha acesso exclusivo ao dado do arquivo
  pthread_mutex_unlock(&mutex); //libera acesso exclusivo do primeiro semáforo
  value = read_data_file();     //faz a leitura do dado
  pthread_mutex_lock(&mutex);   //ganha o acesso exclusivo do primeiro semáforo
  counter = counter - 1;        //subtrai a thread do contador
  if (counter == 0)             //se não houver mais threads
    pthread_mutex_unlock(&db);  //libera o semáforo do dado
  pthread_mutex_unlock(&mutex); //libera o semáforo de controle

  printf("Valor lido: %d\n", value);
  fflush(stdout);
}

void *writer(void *arg)
{ //função das threads de escrita
  pthread_mutex_lock(&db);   //ganha acesso exclusivo ao dado do arquivo
  write_data_file();         //faz a escrita do dado
  pthread_mutex_unlock(&db); //libera o semáforo do dado
}

int main(void)
{
  srand(time(0));
  int i, c_reader = 0, c_writer = 0;

  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&db, NULL);

  //inicializa threads
  for (i = 0; i < M + N; i++)
  {
    if ((i == 0 || i % 2 == 0) && c_writer < M)
    {
      //printf("criando thread de escrita\n");
      pthread_create(&(t[i]), NULL, &writer, NULL);
      c_writer++;
    }
    else if (c_reader < N)
    {
      //printf("criando thread de leitura\n");
      pthread_create(&(t[i]), NULL, &reader, NULL);
    }
  }

  //aguarda todas as threads finalizarem
  for (i = 0; i < M + N; i++)
  {
    printf("executando a thread %d\n", i);
    fflush(stdout);
    pthread_join(t[i], NULL);
  }

  //destroi mutex
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&db);

  return 0;
}