#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"            // Para GetTime()
#include "precise_counter.h"   // Contador preciso (un unico lock)
#include "sloppy_counter.h"    // Contador aproximado sin false sharing

/*
Reproduce, en la maquina en que se ejecute, las dos graficas de OSTEP que
aparecen en las diapositivas:

  1. Preciso vs. aproximado (p. 12 y 24, OSTEP Fig. 29.5): 1, 2, 4, 8 y 16
     hilos, cada uno incrementa COUNT_PER_THREAD veces; aproximado con
     S = 1024. Resultados en results.csv.
  2. Barrido del umbral (p. 25, OSTEP Fig. 29.6): SWEEP_THREADS hilos,
     S = 1, 2, 4, ..., 1024. Resultados en results_threshold.csv.

Compilacion (o simplemente: make):
gcc -Wall -I../include precise_counter.c sloppy_counter.c benchmark_counters.c -pthread -o run_benchmark
*/

#define COUNT_PER_THREAD 1000000   // incrementos por hilo
#define THRESHOLD 1024             // umbral S del experimento 1
#define AMT 1                      // incremento por llamada
#define SWEEP_THREADS 4            // hilos del experimento 2

const int THREAD_COUNTS[] = {1, 2, 4, 8, 16};
const int THRESHOLDS[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
#define LEN(a) ((int)(sizeof(a) / sizeof((a)[0])))

precise_counter_t precise_cnt;
sloppy_counter_t sloppy_cnt;

void *worker_precise(void *arg);
void *worker_sloppy(void *arg);

// Lanza n hilos con la rutina dada y retorna el tiempo transcurrido.
// A cada hilo se le pasa su indice de contador local (i % NUMCPUS).
double run(int n, void *(*routine)(void *)) {
    pthread_t *tid = malloc(sizeof(pthread_t) * n);
    if (tid == NULL) {
        perror("malloc");
        exit(1);
    }
    double t_ini = GetTime();
    for (int i = 0; i < n; i++) {
        pthread_create(&tid[i], NULL, routine, (void *)(long)(i % NUMCPUS));
    }
    for (int i = 0; i < n; i++) {
        pthread_join(tid[i], NULL);
    }
    double t_end = GetTime();
    free(tid);
    return t_end - t_ini;
}

int main() {
    // --- Experimento 1: preciso vs. aproximado al variar los hilos ---
    FILE *fp = fopen("results.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results.csv\n");
        exit(1);
    }
    fprintf(fp, "Threads,CounterType,Time(s),Expected,Actual\n");
    printf("Experimento 1: preciso vs. aproximado (S=%d, %d incrementos por hilo)\n",
           THRESHOLD, COUNT_PER_THREAD);

    for (int t = 0; t < LEN(THREAD_COUNTS); t++) {
        int n = THREAD_COUNTS[t];
        long expected = (long)n * COUNT_PER_THREAD * AMT;
        double time_taken;

        precise_init(&precise_cnt);
        time_taken = run(n, worker_precise);
        printf("  %2d hilos  Preciso:     %.4f s  (valor: %d de %ld)\n",
               n, time_taken, precise_get(&precise_cnt), expected);
        fprintf(fp, "%d,Precise,%.4f,%ld,%d\n", n, time_taken, expected, precise_get(&precise_cnt));

        sloppy_init(&sloppy_cnt, THRESHOLD);
        time_taken = run(n, worker_sloppy);
        printf("  %2d hilos  Aproximado:  %.4f s  (valor: %d de %ld)\n",
               n, time_taken, sloppy_get(&sloppy_cnt), expected);
        fprintf(fp, "%d,Approximate,%.4f,%ld,%d\n", n, time_taken, expected, sloppy_get(&sloppy_cnt));
    }
    fclose(fp);

    // --- Experimento 2: aproximado al variar el umbral S ---
    fp = fopen("results_threshold.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results_threshold.csv\n");
        exit(1);
    }
    fprintf(fp, "Threshold,Time(s),Expected,Actual\n");
    printf("\nExperimento 2: aproximado con %d hilos, variando S\n", SWEEP_THREADS);

    long expected = (long)SWEEP_THREADS * COUNT_PER_THREAD * AMT;
    for (int s = 0; s < LEN(THRESHOLDS); s++) {
        sloppy_init(&sloppy_cnt, THRESHOLDS[s]);
        double time_taken = run(SWEEP_THREADS, worker_sloppy);
        printf("  S=%-5d  %.4f s  (valor: %d de %ld)\n",
               THRESHOLDS[s], time_taken, sloppy_get(&sloppy_cnt), expected);
        fprintf(fp, "%d,%.4f,%ld,%d\n", THRESHOLDS[s], time_taken, expected, sloppy_get(&sloppy_cnt));
    }
    fclose(fp);

    printf("\nBenchmark completado. Resultados en results.csv y results_threshold.csv\n");
    return 0;
}

void *worker_precise(void *arg) {
    for (int i = 0; i < COUNT_PER_THREAD; i++) {
        precise_increment(&precise_cnt);
    }
    return NULL;
}

void *worker_sloppy(void *arg) {
    int threadID = (int)(long)arg;   // indice del contador local
    for (int i = 0; i < COUNT_PER_THREAD; i++) {
        sloppy_update(&sloppy_cnt, threadID, AMT);
    }
    return NULL;
}
