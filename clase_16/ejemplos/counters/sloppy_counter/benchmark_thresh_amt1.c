#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"         // Para GetTime()
#include "sloppy_counter.h" // Contador aproximado (version de las diapositivas)

/*
Barrido del umbral S, como en la grafica "Scaling Sloppy Counters" de las
diapositivas (p. 25): NUM_THREADS hilos, cada uno incrementa el contador
TARGET_COUNT_PER_THREAD veces con amt = 1, para S = 1, 2, 4, ..., 1024.
Guarda los tiempos y el valor obtenido en results_thresh_amt1.csv.

Compilacion (o simplemente: make):
gcc -Wall -I../include sloppy_counter.c benchmark_thresh_amt1.c -pthread -o benchmark_thresh
*/

#define TARGET_COUNT_PER_THREAD 1000000  // un millon por hilo, como en la diapositiva
#define NUM_THREADS 4                    // cuatro hilos, como en la diapositiva
#define AMT 1

const int threshold_values[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

counter_t cnt;

typedef struct {
    counter_t *c;
    int max_value; // Numero de veces que se llama a update()
    int threadID;
    int amt;
} args_t;

void *counting(void *arg);

int main() {
    FILE *fp = fopen("results_thresh_amt1.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results_thresh_amt1.csv para escritura.\n");
        exit(1);
    }

    fprintf(fp, "Threshold,Time(s),Expected,Actual\n");
    printf("Iniciando benchmark (Hilos fijos: %d, Target por hilo: %d, AMT fijo: %d)\n",
           NUM_THREADS, TARGET_COUNT_PER_THREAD, AMT);

    int num_thresholds = sizeof(threshold_values) / sizeof(threshold_values[0]);
    int count_this_run = TARGET_COUNT_PER_THREAD / AMT;  // llamadas a update() por hilo
    long expected_value = (long)NUM_THREADS * count_this_run * AMT;

    for (int t_idx = 0; t_idx < num_thresholds; t_idx++) {
        int threshold = threshold_values[t_idx];

        printf("  Probando Threshold=%-6d ...", threshold);
        fflush(stdout);

        pthread_t tid[NUM_THREADS];
        init(&cnt, threshold);

        double t_ini = GetTime();
        for (int i = 0; i < NUM_THREADS; i++) {
            args_t *args = malloc(sizeof(args_t));
            if (args == NULL) {
                perror("malloc args");
                exit(1);
            }
            args->c = &cnt;
            args->max_value = count_this_run;
            args->amt = AMT;
            args->threadID = i % NUMCPUS;
            pthread_create(&tid[i], NULL, counting, args);
        }
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(tid[i], NULL);
        }
        double t_end = GetTime();

        double time_taken = t_end - t_ini;
        int actual_value = get(&cnt);

        printf(" Tiempo: %.4f s, Real: %d\n", time_taken, actual_value);
        fprintf(fp, "%d,%.4f,%ld,%d\n",
                threshold, time_taken, expected_value, actual_value);
    }

    fclose(fp);
    printf("Benchmark completado. Resultados en 'results_thresh_amt1.csv'\n");
    return 0;
}

// Rutina de cada hilo
void *counting(void *arg) {
  args_t *rvals = (args_t *)arg;
  for (int i = 0; i < rvals->max_value; i++) {
    update(rvals->c, rvals->threadID, rvals->amt);
  }
  free(rvals);
  return NULL;
}
