/*
 * main_singleBuffer-v4.c - La version v3 (buffer de un elemento, empty/fill
 * y while) con marca de terminacion: al final el productor pone un -1 por
 * cada consumidor. Solucion CORRECTA que termina.
 *
 * No aparece en las diapositivas: aplica al buffer de un elemento la marca de
 * terminacion de main_buffer-v4.c (p. 64).
 *
 * Compilacion: gcc -Wall -I. -o main_singleBuffer-v4.out main_singleBuffer-v4.c -pthread
 * Uso:         ./main_singleBuffer-v4.out <loops> <consumers>
 */

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
int verbose = 1; // Para ver lo que saca el consumidor

// -------------------------------------------------------------------------------

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
    Pthread_mutex_lock(&mutex);                // p1
    while (count == 1)                         // p2
      Pthread_cond_wait(&empty, &mutex);       // p3
    put(i);                                    // p4
    Pthread_cond_signal(&fill);                // p5
    Pthread_mutex_unlock(&mutex);              // p6
  }
  // end case: una marca de terminacion (-1) por cada consumidor
  for (i = 0; i < consumers; i++) {
    Mutex_lock(&mutex);
    while (count == 1)
      Cond_wait(&empty, &mutex);
    put(-1);
    Cond_signal(&fill);
    Mutex_unlock(&mutex);
  }
  return NULL;
}

void *consumer(void *arg) {
  int tmp = 0;
  while (tmp != -1) {                          // end case
    Pthread_mutex_lock(&mutex);                // c1
    while (count == 0)                         // c2
      pthread_cond_wait(&fill, &mutex);        // c3
    tmp = get();                               // c4
    pthread_cond_signal(&empty);               // c5
    Pthread_mutex_unlock(&mutex);              // c6
    if (verbose) printf("%d\n", tmp);
  }
  return NULL;
}
