#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "mythreads.h"

#define CMAX 10   // Numero maximo de consumidores

int loops;
int consumers = 1;

// -------------------------------------------------------------------------

int count;  // initially, empty
int buffer;  

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// -------------------------------------------------------------------------

void put(int);
int get(void);
void *producer(void *);
void *consumer(void *);

// -------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (argc != 3) {
      fprintf(stderr, "usage: %s <loops> <consumers>\n", argv[0]);
      exit(1);
    }
    loops = atoi(argv[1]);
    consumers = atoi(argv[2]);
    assert(consumers <= CMAX);
    // Inicializacion del buffer
    buffer = 0;
    count = 0;
    // Hilos
    pthread_t pid;           // Productor
    pthread_t cid[CMAX];     // Consumidor
    
    // Hilo productor
    Pthread_create(&pid, NULL, producer, &loops); 
    // Hilos consumidores
    int i;
    for (i = 0; i < consumers; i++) {
        Pthread_create(&cid[i], NULL, consumer, NULL);
    }
    // Espera del hilo principal
    Pthread_join(pid, NULL); // Espera productor
    // Espera Consumidores
    for (i = 0; i < consumers; i++) {
      Pthread_join(cid[i], NULL);
    }
    return 0;
}

// -------------------------------------------------------------

void put(int value) {
  assert(count == 0);
  count = 1;
  buffer = value;
}

int get(void) {
  assert(count == 1);
  count = 0;
  return buffer;
}

// -------------------------------------------------------------

void *producer(void *arg) {
  int i;
  int loops = *(int *)arg;
  for (i = 0; i < loops; i++) {
    put(i);
  }
  return NULL;
}

void *consumer(void *arg) {
  while (1) {
    int tmp = get();
    printf("%d\n", tmp);
  }
  return NULL;
}

