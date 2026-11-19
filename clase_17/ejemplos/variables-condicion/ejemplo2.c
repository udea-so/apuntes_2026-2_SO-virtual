/*
 * ejemplo2.c - El padre espera al hijo con una variable de condicion (join).
 *
 * Diapositivas p. 17-22 (Implementacion del join); OSTEP cap. 30, Fig. 30.3.
 *
 * Compilacion: gcc -Wall -I. -o ejemplo2.out ejemplo2.c -pthread
 * Uso:         ./ejemplo2.out [segundos]
 *
 *   segundos (opcional, por defecto 0): tiempo que el hijo duerme antes de
 *   imprimir "child". Con el mismo valor que en ejemplo1.out permite comparar
 *   con `time` la CPU que gasta el padre mientras espera.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int espera = 0;   // segundos que el hijo tarda en terminar

// Prototipos de las funciones
void thr_exit(void);
void *child(void *);
void thr_join(void);


// Funcion main

int main(int argc, char *argv[]) {
  if (argc > 1)
    espera = atoi(argv[1]);
  printf("parent: begin\n");
  pthread_t p;
  Pthread_create(&p, NULL, child, NULL);
  thr_join();
  printf("parent: end\n");
  return 0;
}

// Definiciones de las funciones

void thr_exit(void) {
  Pthread_mutex_lock(&m);
  done = 1;
  Pthread_cond_signal(&c);
  Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
  if (espera > 0)
    sleep(espera);
  printf("child\n");
  thr_exit();
  return NULL;
}

void thr_join(void) {
  Pthread_mutex_lock(&m);
  while (done == 0) {
    Pthread_cond_wait(&c, &m);
  }
  Pthread_mutex_unlock(&m);
}
