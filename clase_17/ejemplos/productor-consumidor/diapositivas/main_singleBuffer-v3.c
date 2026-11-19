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

pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// -------------------------------------------------------------------------------

int count;  // initially, empty
int buffer;  

void put(int);
int get(void);

void *producer(void *);
void *consumer(void *);

// -------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------

void *producer(void *arg) {
  int i;
  int loops = *(int *)arg;
  for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);     	          // p1
    while(count == 1)                           // p2
      Pthread_cond_wait(&empty, &mutex);         // p3
    put(i);                                     // p4
    Pthread_cond_signal(&fill);                 // p5
    Pthread_mutex_unlock(&mutex);     	        // p6
  }
}

void *consumer(void *arg) {
  int tmp;
  while (1) {    
    Pthread_mutex_lock(&mutex);     	          // c1
    while(count == 0)                           // p2
      pthread_cond_wait(&fill, &mutex);         // c3
    tmp = get();                                // c4   
    pthread_cond_signal(&empty);       	        // c5
    Pthread_mutex_unlock(&mutex);     	        // c6
    printf("%d\n", tmp);
  }
}

