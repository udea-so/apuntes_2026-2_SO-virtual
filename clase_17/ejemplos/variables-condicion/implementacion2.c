/*
 * implementacion2.c - Reimplementacion 2 del join: con variable de estado
 * (done), pero thr_exit() NO adquiere el lock.
 *
 * Diapositivas p. 27-29 y 34; OSTEP cap. 30, Fig. 30.5 (Parent Waiting: No
 * Lock). Esta implementacion esta ROTA: tiene una condicion de carrera.
 *
 * Compilacion: gcc -Wall -I. -o implementacion2.out implementacion2.c -pthread
 * Uso:         ./implementacion2.out [milisegundos]
 *
 *   milisegundos (opcional, por defecto 0): pausa que hace el padre entre la
 *   verificacion de done (x) y la llamada a wait (y). Simula que el padre es
 *   interrumpido justo en ese punto (caso 2 de la p. 29): el hijo cambia done
 *   y envia la senal antes de que el padre duerma, y el padre espera por
 *   siempre. Se termina con Ctrl+C.
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
  if (done == 0) {                 // x
    if (ventana_ms > 0)
      usleep(ventana_ms * 1000);   // el padre "es interrumpido" entre x e y
    pthread_cond_wait(&c, &m);     // y
  }
  pthread_mutex_unlock(&m);        // z
}


void thr_exit(void) {
  done = 1;                   // a
  pthread_cond_signal(&c);    // b
}

void *child(void *arg) {
  printf("child\n");
  thr_exit();
  return NULL;
}
