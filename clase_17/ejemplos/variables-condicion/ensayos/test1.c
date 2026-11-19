/*
 * test1.c - Ensayos con wait y signal sobre el join correcto (ejemplo2.c).
 *
 * Sin opciones, el programa es el join correcto. Cada variante cambia una
 * sola cosa y se activa al compilar con -D (el Makefile las genera todas):
 *
 *   -DSIN_SIGNAL    thr_exit() no llama a signal.
 *   -DDOBLE_SIGNAL  thr_exit() llama dos veces a signal.
 *   -DWAIT_EN_MAIN  main() llama a wait antes de crear al hijo, sin tener
 *                   el lock.
 *
 * Compilacion: gcc -Wall -I. -o test1.out test1.c -pthread
 *              gcc -Wall -I. -DSIN_SIGNAL -o test1-sin-signal.out test1.c -pthread
 * Uso:         ./test1.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

// Prototipos de las funciones
void thr_exit(void);
void *child(void *);
void thr_join(void);


// Funcion main

int main(int argc, char *argv[]) {
  printf("parent: begin\n");
#ifdef WAIT_EN_MAIN
  Pthread_cond_wait(&c, &m);   // wait sin tener el lock y sin hijo que senale
#endif
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
#ifndef SIN_SIGNAL
  Pthread_cond_signal(&c);
#endif
#ifdef DOBLE_SIGNAL
  Pthread_cond_signal(&c);
#endif
  Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
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
