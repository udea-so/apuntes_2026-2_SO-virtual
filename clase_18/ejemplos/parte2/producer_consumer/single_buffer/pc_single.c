#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include <semaphore.h>

/** ----------------------- Variables globales ------------------------- **/
#define CMAX 10
#define PMAX 10

int consumers;
int producers;

int buffer;
int fill = 0;
int use = 0;
int loops = 0;

/** ----------------------- Variables de sincronizacion ------------------------- **/
// Semaforos
sem_t empty;
sem_t full;
/** ------------------------------------------------------------------------------ **/

/** --------------------------- Funciones del buffer ----------------------------- **/
void put(int value)
{
  buffer = value;    // Line F1
}

int get()
{
  int tmp = buffer; // Line G1
  return tmp;
}

/** ------------------- Funciones del productor y consumidor ---------------------- **/

void *producer(void *arg) {
  int num_producer = *((int *)arg) + 1;
  printf("Begin -> Producer %d\n", num_producer);
  //printf("Begin -> Producer[%lX]\n", pthread_self());
  int i;
  for (i = 0; i < loops; i++)
  {
    sem_wait(&empty); // Line P1
    put(i);           // Line P2
    sem_post(&full);  // Line P3
  }

  // end case
  for (i = 0; i < consumers; i++)
  {
    sem_wait(&empty);
    put(-1);
    sem_post(&full);
  }
  printf("End -> Producer %d\n", num_producer);
  return NULL;
}

void *consumer(void *arg) {
  int num_consumer = *((int *)arg) + 1;
  printf("Begin -> Consumer %d\n", num_consumer);
  int tmp = 0;
  while (tmp != -1) {
    sem_wait(&full);  // Line C1
    tmp = get();      // Line C2
    sem_post(&empty); // Line C3
    printf("[%d] <- %d\n", num_consumer , tmp);
  }
  printf("End -> Consumer %d\n", num_consumer);
  return NULL;
}

/** ----------------------- Funcion Main -------------------------- **/
int main(int argc, char *argv[]) {
  
  /** ----------- Pruebas -------- */ 
  int i;
  loops = 2;    // Cambiar
  producers = 1;  // Cambiar 
  consumers = 1;  // Cambiar

  assert(consumers <= CMAX);
  assert(producers <= PMAX);
  printf("Parent: begin\n");

  // -------------  Inicializacion de los semaforos  --------------
  sem_init(&empty, 0, 1);   // 1 are empty
  sem_init(&full, 0, 0);    // 0 are full

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
