#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"
#include "sloppy_counter.h"

/*
Contador aproximado: NUM_THREADS hilos llaman CNT_END veces a update()
con amt = AMT y umbral S = THRESHOLD. El hilo i usa el contador local
i % NUMCPUS (con 16 hilos y NUMCPUS = 8, dos hilos comparten cada local).

Compilacion (o simplemente: make):
gcc -Wall -I../include sloppy_counter.c main_sloppy_counter.c -pthread -o sloppy_example
*/

#define CNT_END 1000000
#define NUM_THREADS 16
#define THRESHOLD 1024
#define AMT 1

counter_t cnt;  // contador compartido por todos los hilos

// Argumentos de cada hilo
typedef struct {
    counter_t *c;
    int max_value;   // veces que el hilo llama a update()
    int threadID;    // indice del contador local que usa
    int amt;         // cantidad que suma en cada llamada
} args_t;

void *counting(void *);

int main() {
  int i;
  double t_ini, t_end;
  pthread_t tid[NUM_THREADS];

  init(&cnt, THRESHOLD);

  printf("Iniciando prueba con %d hilos (Threshold=%d)...\n", NUM_THREADS, THRESHOLD);

  t_ini = GetTime(); // Marca de tiempo inicial
  for (i = 0; i < NUM_THREADS; i++) {
    // Cada hilo recibe su propia copia de los argumentos (en el heap)
    args_t *args = malloc(sizeof(args_t));
    if (args == NULL) {
        perror("malloc");
        exit(1);
    }
    args->c = &cnt;
    args->amt = AMT;
    args->max_value = CNT_END;
    args->threadID = i % NUMCPUS;
    pthread_create(&tid[i], NULL, counting, args);
  }
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }
  t_end = GetTime(); // Marca de tiempo final

  // Lo que quedo en los contadores locales sin llegar al umbral.
  // Se lee sin locks porque todos los hilos ya terminaron.
  int pending = 0;
  for (i = 0; i < NUMCPUS; i++) {
    pending += cnt.local[i];
  }

  printf("-> Tiempo gastado: %lf seg\n", t_end - t_ini);
  printf("-> El contador debe quedar en: %d\n", NUM_THREADS * CNT_END);
  printf("-> El valor aproximado del contador es: %d\n", get(&cnt));
  printf("-> Incrementos que siguen en los contadores locales: %d\n", pending);
  return 0;
}

// Rutina de cada hilo
void *counting(void *args) {
  args_t *rvals = (args_t *)args;
  for (int i = 0; i < rvals->max_value; i++) {
    update(rvals->c, rvals->threadID, rvals->amt);
  }
  free(rvals); // libera los argumentos reservados en main
  return NULL;
}
