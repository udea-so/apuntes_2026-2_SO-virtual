#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"         // Para GetTime()
#include "sloppy_counter.h" // Contador aproximado (version de las diapositivas)

/*
Mide el contador aproximado con 1, 2, 4, 8 y 16 hilos (cada uno llama
COUNT_PER_THREAD veces a update()) con el umbral S = 1024 de la grafica de
las diapositivas (p. 24) y guarda los tiempos en results_sloppy.csv.

Compilacion (o simplemente: make):
gcc -Wall -I../include sloppy_counter.c benchmark_sloppy_counter.c -pthread -o benchmark_sloppy
*/

#define COUNT_PER_THREAD 1000000
#define THRESHOLD 1024   // umbral S
#define AMT 1            // incremento por llamada

counter_t cnt;

typedef struct {
    counter_t *c;
    int max_value;
    int threadID;
    int amt;
} args_t;

void *counting(void *arg);

int main() {
    FILE *fp = fopen("results_sloppy.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results_sloppy.csv para escritura.\n");
        exit(1);
    }

    fprintf(fp, "Threads,Time(s),Expected,Actual\n");
    printf("Iniciando benchmark para 'sloppy_counter' (Threshold=%d)...\n", THRESHOLD);
    printf("Resultados se guardarán en results_sloppy.csv\n");

    const int thread_counts[] = {1, 2, 4, 8, 16};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    for (int t = 0; t < num_tests; t++) {
        int num_threads = thread_counts[t];
        long expected_value = (long)num_threads * COUNT_PER_THREAD * AMT;

        printf("  Probando con %d hilos...\n", num_threads);

        pthread_t *tid = malloc(sizeof(pthread_t) * num_threads);
        if (tid == NULL) {
            fprintf(stderr, "Error: No se pudo reservar memoria para los hilos.\n");
            exit(1);
        }

        init(&cnt, THRESHOLD);  // reinicia el contador en cada prueba

        double t_ini = GetTime();
        for (int i = 0; i < num_threads; i++) {
            args_t *args = malloc(sizeof(args_t));
            if (args == NULL) {
                perror("malloc args");
                exit(1);
            }
            args->c = &cnt;
            args->max_value = COUNT_PER_THREAD;
            args->amt = AMT;
            args->threadID = i % NUMCPUS;  // contador local que usa el hilo
            pthread_create(&tid[i], NULL, counting, args);
        }
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        double t_end = GetTime();

        double time_taken = t_end - t_ini;
        int actual_value = get(&cnt);  // solo el global: valor aproximado

        printf("    Tiempo: %.4f s, Esperado: %ld, Real: %d\n",
               time_taken, expected_value, actual_value);
        fprintf(fp, "%d,%.4f,%ld,%d\n",
                num_threads, time_taken, expected_value, actual_value);

        free(tid);
    }

    fclose(fp);
    printf("Benchmark completado.\n");
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
