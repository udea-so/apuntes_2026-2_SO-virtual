#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "counter.h"

/*
Contador SIN lock (diapositivas p. 6): NUM_THREADS hilos incrementan
CNT_END veces un mismo contador compartido.

Compilacion (o simplemente: make):
gcc -Wall -I../include counter.c counter_without_locks.c -pthread -o bad_counter
*/

#define CNT_END 1000000
#define NUM_THREADS 8

counter_t cnt;  // contador compartido por todos los hilos

void *counting(void *);

int main() {
  int i;
  double t_ini, t_end;
  pthread_t tid[NUM_THREADS];

  init(&cnt);

  t_ini = GetTime(); // Marca de tiempo inicial
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_create(&tid[i], NULL, counting, &cnt);
  }
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }
  t_end = GetTime(); // Marca de tiempo final

  printf("-> Tiempo gastado: %lf seg\n", t_end - t_ini);
  printf("-> El contador debe quedar en: %d\n", NUM_THREADS * CNT_END);
  printf("-> El valor real del contador es: %d\n", get(&cnt));
  return 0;
}

// Rutina de cada hilo: incrementa CNT_END veces el contador recibido
void *counting(void *cnt_t) {
  counter_t *r_cnt = (counter_t *)cnt_t;
  for (int i = 0; i < CNT_END; i++) {
    increment(r_cnt);
  }
  return NULL;
}
