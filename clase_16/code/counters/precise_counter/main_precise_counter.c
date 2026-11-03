#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "counter_lock.h"

#define CNT_END 1000000
#define NUM_THREADS 16

/*
gcc -Wall -I../include counter_lock.c precise_counter.c -lpthread -o precise_counter
*/

int real_value = 0;
counter_t cnt;
int cnt_value;
int num_threads;

// Prototipos de las funciones

void *counting(void *);

// Funcion main

int main() {
  init(&cnt);
  int i;
  double t_ini, t_end;

  pthread_t tid[NUM_THREADS];

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

// Definiciones de las funciones

void *counting(void *cnt_t) {
  counter_t *r_cnt = (counter_t *)cnt_t;
  for (int i = 0; i < CNT_END; i++) {
    increment(r_cnt);
  }
  return NULL;
}