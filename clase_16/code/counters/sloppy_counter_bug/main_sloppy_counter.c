#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "common.h"
#include "common_threads.h"
#include "sloppy_counter.h"

#define CNT_END 1000000
#define NUM_THREADS 16
#define THRESHOLD 1024
#define AMT 1

/*
gcc -Wall -I../include sloppy_counter.c  test_sloppy_counter.c -lpthread -o sloppy_counter
*/

int real_value = 0;
counter_t cnt;
int cnt_value;
int num_threads;

typedef struct {
    counter_t *c;
    int max_value;
    int threadID;
    int amt;
} args_t;


// Prototipos de las funciones

void *counting(void *);

// Funcion main

int main() {
  // Inicializacion del contador aproximado
  init(&cnt, THRESHOLD);

  int counter_by_thread = CNT_END;
  double t_ini, t_end;
  pthread_t tid[NUM_THREADS]; // Array en el stack para los IDs de hilos

  int i;
  printf("Iniciando prueba con %d hilos (Threshold=%d)...\n", NUM_THREADS, THRESHOLD);
  
  t_ini = GetTime(); // Marca de tiempo inicial

  for (i = 0; i < NUM_THREADS; i++) {
    
    // Asignar memoria para los argumentos de CADA hilo!
    args_t *args = malloc(sizeof(args_t));
    if (args == NULL) {
        perror("malloc");
        exit(1);
    }

    // Inicializando argumentos
    args->c = &cnt;
    args->amt = AMT;
    args->max_value = counter_by_thread;
    args->threadID = i % NUMCPUS; // Cada hilo obtiene su propio 'i'

    // Creacion del hilo
    pthread_create(&tid[i], NULL, counting, args);
  }

  // --- Espera (Join) de Hilos ---
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }

  t_end = GetTime(); // Marca de tiempo final

  printf("-> Tiempo gastado: %lf seg\n", t_end - t_ini);
  printf("-> El contador debe quedar en: %d\n", NUM_THREADS * CNT_END);
  printf("-> El valor aproximado del contador es: %d\n", get(&cnt));

  return 0;
}

// Definiciones de las funciones

/* Function Thread */
void *counting(void *args) {
    args_t *rvals = (args_t*)args;    
    // Bucle de conteo
    for(int i = 0; i < rvals->max_value; i++) {
        update(rvals->c,rvals->threadID,rvals->amt);
    }
    // Ojo: No olvidar liberar la memoria de los argumentos asignados en el main
    free(rvals);
    return NULL;
}

