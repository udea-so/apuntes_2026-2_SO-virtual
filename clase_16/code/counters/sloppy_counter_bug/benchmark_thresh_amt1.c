#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>     // Para intptr_t
#include <unistd.h>     // Para sysconf

#include "common.h"         // Para GetTime()
#include "sloppy_counter.h" // Nuestro contador APROXIMADO

/*
Compilación sugerida:
(Asumiendo que common.h está en ../include y tiene 'static inline GetTime')
gcc -Wall -I../include sloppy_counter.c benchmark_thresh_amt1.c -lpthread -o benchmark_amt1
*/

// --- CONFIGURACIÓN DEL BENCHMARK ---

// Valor total que CADA hilo debe sumar
#define TARGET_COUNT_PER_THREAD 10000000

// Usamos un número fijo de hilos, igual al de CPUs
// NUMCPUS se define en sloppy_counter.h (probablemente como 8)
#define NUM_THREADS NUMCPUS

// --- VALOR FIJO DE AMT ---
#define AMT 1

// --- Valores a probar (Eje X de la gráfica) ---
const int threshold_values[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};


// El contador global
counter_t cnt;

// --- ESTRUCTURA DE ARGUMENTOS ---
typedef struct {
    counter_t *c;
    int max_value; // Número de veces que llamaremos a update()
    int threadID;
    int amt;
} args_t;

// Prototipos
void *counting(void *arg);
int get_precise(counter_t *c);

int main() {
    // Archivo de salida solo para amt=1
    FILE *fp = fopen("results_thresh_amt1.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results_thresh_amt1.csv para escritura.\n");
        exit(1);
    }

    // Escribir el encabezado del CSV (solo Threshold y Tiempo)
    fprintf(fp, "Threshold,Time(s),Expected,Actual\n");
    printf("Iniciando benchmark (Hilos fijos: %d, Target por hilo: %d, AMT fijo: %d)\n", 
           NUM_THREADS, TARGET_COUNT_PER_THREAD, AMT);

    int num_thresholds = sizeof(threshold_values) / sizeof(threshold_values[0]);
    const int num_threads = NUM_THREADS; // Fijo
    const int amt = AMT;                 // Fijo

    // Bucle interno: Itera sobre THRESHOLD (Eje X)
    for (int t_idx = 0; t_idx < num_thresholds; t_idx++) {
        int threshold = threshold_values[t_idx];

        // Con amt=1, el número de llamadas es igual al total
        int count_this_run = TARGET_COUNT_PER_THREAD / amt;
        
        long expected_value = (long)num_threads * count_this_run * amt;
        
        printf("  Probando Threshold=%-6d ...", threshold);
        fflush(stdout); // Forzar la impresión

        pthread_t *tid = malloc(sizeof(pthread_t) * num_threads);
        if (tid == NULL) {
            perror("malloc tid");
            exit(1);
        }

        // --- Ejecución de la prueba ---
        init(&cnt, threshold); 
        
        double t_ini = GetTime();
        
        for (int i = 0; i < num_threads; i++) {
            args_t *args = malloc(sizeof(args_t));
            args->c = &cnt;
            args->max_value = count_this_run; // Veces que se llama a update()
            args->amt = amt;
            args->threadID = i % NUMCPUS; // (i % 8)
            
            pthread_create(&tid[i], NULL, counting, args);
        }
        
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        
        double t_end = GetTime();
        
        // --- Recolección de resultados ---
        double time_taken = t_end - t_ini; // <-- Eje Y
        int actual_value = get(&cnt);

        printf(" Tiempo: %.4f s\n", time_taken);
        
        // Escribir al archivo CSV
        fprintf(fp, "%d,%.4f,%ld,%d\n", 
                threshold, time_taken, expected_value, actual_value);

        free(tid);
    }

    fclose(fp);
    printf("Benchmark completado. Resultados en 'results_thresh_amt1.csv'\n");
    return 0;
}

void *counting(void *arg) { 
  // 1. Recibir los argumentos
  args_t *rvals = (args_t *)arg;
  
  // 2. Bucle de conteo usando la función update
  for (int i = 0; i < rvals->max_value; i++) {
    update(rvals->c, rvals->threadID, rvals->amt);
  } 
  
  // 3. Liberar la memoria de los argumentos
  free(rvals); 
  return NULL; 
}