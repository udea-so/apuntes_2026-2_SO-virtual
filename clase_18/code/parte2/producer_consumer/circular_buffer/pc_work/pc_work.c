#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include <semaphore.h>

/** ----------------------- Variables globales ------------------------- **/
#define MAX 10
#define CMAX 10
#define PMAX 10

int consumers;
int producers;

int buffer[MAX];
int fill = 0;
int use = 0;
int loops = 0;

/** ----------------------- Variables de sincronizacion ------------------------- **/
// Semaforos
sem_t empty;
sem_t full;
sem_t mutex;
/** ------------------------------------------------------------------------------ **/

/** --------------------------- Funciones del buffer ----------------------------- **/
void put(int value)
{
  buffer[fill] = value;    // Line F1
  fill = (fill + 1) % MAX; // Line F2
}

int get()
{
  int tmp = buffer[use]; // Line G1
  use = (use + 1) % MAX; // Line G2
  return tmp;
}

/** ------------------- Funciones del productor y consumidor ---------------------- **/

void *producer(void *arg) {
  int num_producer = *((int *)arg) + 1;
  printf("Begin -> Producer %d\n", num_producer);
  int i;
  for (i = 0; i < loops; i++)
  {
    sem_wait(&empty); // Line P1
    sem_wait(&mutex); // Line P1.5 (MUTEX HERE)
    put(i); // Line P2
    sem_post(&mutex); // Line P2.5 (AND HERE)
    sem_post(&full); // Line P3
    printf("P%d: B[%d] <- %d\n", num_producer , fill, i);
  }

  // end case
  for (i = 0; i < consumers; i++)
  {
    sem_wait(&empty);
    sem_wait(&mutex);
    put(-1);
    sem_post(&mutex); 
    sem_post(&full);
    printf("P%d: B[%d] <- %d\n", num_producer , fill, -1);
  }
  printf("End -> Producer %d\n", num_producer);
  return NULL;
}

void *consumer(void *arg)
{
  int num_consumer = *((int *)arg) + 1;
  printf("Begin -> Consumer %d\n", num_consumer);
  int tmp = 0;
  while (tmp != -1)
  {
    sem_wait(&full); // Line C1
    sem_wait(&mutex); // Line C1.5 (MUTEX HERE)
    tmp = get(); // Line C2
    sem_post(&mutex); // Line C2.5 (AND HERE)
    sem_post(&empty); // Line C3
    printf("[%d] <- %d\n", num_consumer, tmp);
  }
  printf("End -> Consumer %d\n", num_consumer);
  return NULL;
}

/**
 * CONCLUSION: El codigo nunca funciona cuando hay mas de un consumidor
 * 
 */

/** ----------------------- Funcion Main -------------------------- **/
int main(int argc, char *argv[]) {

  int i;
  loops = 50;    // Cambiar
  producers = 1;  // Cambiar 
  consumers = 2;  // Cambiar

  assert(consumers <= CMAX);
  assert(producers <= PMAX);
  printf("Parent: begin\n");

  // -------------  Inicializacion de los semaforos  --------------
  sem_init(&empty, 0, MAX); // MAX are empty
  sem_init(&full, 0, 0);    // 0 are full
  sem_init(&mutex , 0 , 1);// mutex = 1
  // ----------------- Declaracion de los Hilos  -----------------
  pthread_t pid[PMAX], cid[CMAX];
  // ----------------- Inicializacion -----------------
  for (i = 0; i < producers; i++) {
    Pthread_create(&pid[i], NULL, producer, (void*)&i);
  }
  for (i = 0; i < consumers; i++) {
    Pthread_create(&cid[i], NULL, consumer, (void*)&i);
  }

  // -----------------  Espera  -----------------
  for (i = 0; i < producers; i++) {
    Pthread_join(pid[i], NULL);
  }
  for (i = 0; i < consumers; i++) {
    Pthread_join(cid[i], NULL);
  }

  printf("Parent: end\n");
  return 0;
}
