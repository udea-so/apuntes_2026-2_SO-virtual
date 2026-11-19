/*
 * implementacion1.c - Reimplementacion 1 del join: SIN variable de estado.
 *
 * Diapositivas p. 24-26 y 33; OSTEP cap. 30, Fig. 30.4 (Parent Waiting: No
 * State Variable). Esta implementacion esta ROTA.
 *
 * Compilacion: gcc -Wall -I. -o implementacion1.out implementacion1.c -pthread
 * Uso:         ./implementacion1.out [segundos]
 *
 *   segundos (opcional, por defecto 0): tiempo que el padre duerme despues de
 *   crear al hijo y antes de llamar a thr_join(). Con un valor > 0 se fuerza
 *   el caso 2 de la p. 26 (el hijo corre primero) y el padre espera por
 *   siempre. Se termina con Ctrl+C.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

// Prototipos de las funciones
void thr_exit(void);
void *child(void *);
void thr_join(void);


// Funcion main

int main(int argc, char *argv[]) {
  int retardo = (argc > 1) ? atoi(argv[1]) : 0;
  printf("parent: begin\n");
  pthread_t p;
  Pthread_create(&p, NULL, child, NULL);
  if (retardo > 0)
    sleep(retardo); // el hijo termina antes de que el padre espere
  thr_join();
  printf("parent: end\n");
  return 0;
}

// Definiciones de las funciones

void thr_join(void) {
  pthread_mutex_lock(&m);      // x
  pthread_cond_wait(&c, &m);   // y
  pthread_mutex_unlock(&m);    // z
}

void thr_exit(void) {
  pthread_mutex_lock(&m);      // a
  pthread_cond_signal(&c);     // b
  pthread_mutex_unlock(&m);    // c
}

void *child(void *arg) {
  printf("child\n");
  thr_exit();
  return NULL;
}
