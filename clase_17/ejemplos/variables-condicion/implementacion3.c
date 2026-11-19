/*
 * implementacion3.c - Reimplementacion 3 del join: variable de estado, lock
 * en ambos lados y while. Implementacion CORRECTA.
 *
 * Diapositivas p. 30-31 y 35; OSTEP cap. 30, Fig. 30.3.
 *
 * Compilacion: gcc -Wall -I. -o implementacion3.out implementacion3.c -pthread
 * Uso:         ./implementacion3.out [milisegundos]
 *
 *   milisegundos (opcional, por defecto 0): la misma pausa entre x e y que en
 *   implementacion2.c. Aqui el programa termina siempre: mientras el padre
 *   tiene el lock, el hijo no puede ejecutar thr_exit().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int ventana_ms = 0;   // pausa entre x e y (0 = sin pausa)

// Prototipos de las funciones
void thr_exit(void);
void *child(void *);
void thr_join(void);


// Funcion main

int main(int argc, char *argv[]) {
  if (argc > 1)
    ventana_ms = atoi(argv[1]);
  printf("parent: begin\n");
  pthread_t p;
  Pthread_create(&p, NULL, child, NULL);
  thr_join();
  printf("parent: end\n");
  return 0;
}

// Definiciones de las funciones

void thr_join(void) {
  pthread_mutex_lock(&m);          // w
  while (done == 0) {              // x
    if (ventana_ms > 0)
      usleep(ventana_ms * 1000);   // el padre "es interrumpido" entre x e y
    pthread_cond_wait(&c, &m);     // y
  }
  pthread_mutex_unlock(&m);        // z
}

void thr_exit(void) {
  pthread_mutex_lock(&m);     // a
  done = 1;                   // b
  pthread_cond_signal(&c);    // c
  pthread_mutex_unlock(&m);   // d
}

void *child(void *arg) {
  printf("child\n");
  thr_exit();
  return NULL;
}
