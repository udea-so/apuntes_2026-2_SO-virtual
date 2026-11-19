/*
 * main_singleBuffer-v1.c - Productor/consumidor con buffer de un elemento,
 * una sola variable de condicion y sentencia if. Solucion ROTA.
 *
 * Diapositivas p. 42-49; OSTEP cap. 30, Fig. 30.8 y traza de la Fig. 30.9.
 *
 * Compilacion: gcc -Wall -I. -o main_singleBuffer-v1.out main_singleBuffer-v1.c -pthread
 * Con traza:   gcc -Wall -I. -DTRAZA -o main_singleBuffer-v1-traza.out main_singleBuffer-v1.c -pthread
 * Uso:         ./main_singleBuffer-v1.out <loops> <consumers>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "mythreads.h"

#define CMAX 10   // Numero maximo de consumidores

// Traza opcional (compilar con -DTRAZA): imprime los pasos de cada hilo con
// las etiquetas p1-p6 / c1-c6 de las diapositivas en lugar de los valores.
#ifdef TRAZA
#define TRAZA_PRINTF(...) printf(__VA_ARGS__)
#else
#define TRAZA_PRINTF(...)
#endif

int loops;
int consumers = 1;

// -------------------------------------------------------------------------------

int count;  // initially, empty
int buffer;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// -------------------------------------------------------------------------------

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
    pthread_t cid[CMAX];     // Consumidores
    int ids[CMAX];           // Identificador de cada consumidor (Tc1, Tc2, ...)

    // Hilo productor
    Pthread_create(&pid, NULL, producer, &loops);
    // Hilos consumidores
    int i;
    for (i = 0; i < consumers; i++) {
        ids[i] = i + 1;
        Pthread_create(&cid[i], NULL, consumer, &ids[i]);
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
    if (count == 1) {                          // p2
      TRAZA_PRINTF("Tp  p3: buffer lleno, duerme\n");
      Pthread_cond_wait(&cond, &mutex);        // p3
      TRAZA_PRINTF("Tp  p3: despierta (count=%d)\n", count);
    }
    put(i);                                    // p4
    TRAZA_PRINTF("Tp  p4: produce %d\n", i);
    TRAZA_PRINTF("Tp  p5: signal\n");
    Pthread_cond_signal(&cond);                // p5
    Pthread_mutex_unlock(&mutex);              // p6
  }
  return NULL;
}

void *consumer(void *arg) {
  int id = *(int *)arg;   // 1, 2, ... (Tc1, Tc2, ...); solo lo usa la traza
  (void) id;
  int tmp;
  while (1) {
    Pthread_mutex_lock(&mutex);                // c1
    if (count == 0) {                          // c2
      TRAZA_PRINTF("Tc%d c3: buffer vacio, duerme\n", id);
      pthread_cond_wait(&cond, &mutex);        // c3
      TRAZA_PRINTF("Tc%d c3: despierta (count=%d)\n", id, count);
    }
    tmp = get();                               // c4
    TRAZA_PRINTF("Tc%d c4: consume %d\n", id, tmp);
    TRAZA_PRINTF("Tc%d c5: signal\n", id);
    pthread_cond_signal(&cond);                // c5
    Pthread_mutex_unlock(&mutex);              // c6
#ifndef TRAZA
    printf("%d\n", tmp);
#endif
  }
  return NULL;
}
