#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

/*
gcc -Wall -I. test1.c -lpthread -o test1
*/

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
  // Pthread_cond_wait(&c, &m);  // Que pasa si se descomenta?
  pthread_t p;
  pthread_create(&p, NULL, child, NULL);
  thr_join();
  printf("parent: end\n");
  return 0;
}

// Definiciones de las funciones

void thr_exit(void) {
  Pthread_mutex_lock(&m);
  done = 1;
  Pthread_cond_signal(&c);  // Que pasa si se comenta?
  // Pthread_cond_signal(&c); // Que pasa si se quita el comentario?
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
