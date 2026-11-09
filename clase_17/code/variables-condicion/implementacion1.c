#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

/*
gcc -Wall -I. implementacion1.c -o implementacion1 -lpthread
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
  pthread_t p;
  Pthread_create(&p, NULL, child, NULL);  
  // sleep(2); 
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


