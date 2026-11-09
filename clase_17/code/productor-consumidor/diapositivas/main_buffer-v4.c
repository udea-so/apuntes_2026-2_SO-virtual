#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "mythreads.h"

/*
links: https://unstop.com/blog/void-pointer-in-c
*/

#define CMAX 10   // Numero maximo de consumidores

int loops;
int max;
int consumers = 1;
int verbose = 1;

// -------------------------------------------------------------------------------

pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// -------------------------------------------------------------------------------

int *buffer;  // have to allocate space

int count = 0;  // initially, empty
int fill_ptr = 0;
int use_ptr = 0;

void put(int);
int get(void);

void *producer(void *);
void *consumer(void *);

// -------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (argc != 4) {
      fprintf(stderr, "usage: %s <buffersize> <loops> <consumers>\n", argv[0]);
      exit(1);
    }
    max   = atoi(argv[1]);
    loops = atoi(argv[2]);
    consumers = atoi(argv[3]);
    assert(consumers <= CMAX);

    // Inicializacion del buffer
    buffer = (int *) malloc(max * sizeof(int));
    int i;
    for (i = 0; i < max; i++) { 
      buffer[i] = 0;
    }
    // Hilos
    pthread_t pid;           // Productor
    pthread_t cid[CMAX];     // Consumidor
    
    // Hilo productor
    Pthread_create(&pid, NULL, producer, &loops); 
    // Hilos consumidores
    for (i = 0; i < consumers; i++) {
        Pthread_create(&cid[i], NULL, consumer, NULL);
    }
    // Espera del hilo principal
    Pthread_join(pid, NULL); // Espera productor
    // Espera Consumidores
    for (i = 0; i < consumers; i++) {
      Pthread_join(cid[i], NULL);
    }
    free(buffer);
    return 0;
}

// -------------------------------------------------------------------------------

void put(int value) {
  buffer[fill_ptr] = value;
  fill_ptr = (fill_ptr + 1)%max;
  count++;
}

int get(void) {
  int tmp = buffer[use_ptr];
  use_ptr = (use_ptr + 1)%max;
  count--;
  return tmp;
}

// -------------------------------------------------------------------------------

void *producer(void *arg) {
  int i;
  int loops = *(int *)arg;
  for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);     	           // p1
    while(count == max)                            // p2
      Pthread_cond_wait(&empty, &mutex);         // p3
    put(i);                                      // p4
    Pthread_cond_signal(&fill);                  // p5
    Pthread_mutex_unlock(&mutex);     	         // p6
  }

  // end case
  for (i = 0; i < consumers; i++) {
    Mutex_lock(&mutex);
    while (count == max) 
      Cond_wait(&empty, &mutex);
	  put(-1);
	  Cond_signal(&fill);
	  Mutex_unlock(&mutex);
  }
  return NULL;
}

void *consumer(void *arg) {
  int tmp = 0;
  while (tmp != -1) {                           // end case
    Pthread_mutex_lock(&mutex);     	          // c1
    while(count == 0)                           // p2
      pthread_cond_wait(&fill, &mutex);         // c3
    tmp = get();                                // c4   
    pthread_cond_signal(&empty);       	        // c5
    Pthread_mutex_unlock(&mutex);     	        // c6
    if (verbose) printf("%d\n", tmp);
  }
  return NULL;
}

