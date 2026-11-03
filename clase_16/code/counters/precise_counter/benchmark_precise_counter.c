#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>     // Para intptr_t
#include <unistd.h>     // Para sysconf

#include "common.h"         // Para GetTime()
#include "counter_lock.h"   // Nuestro contador preciso

/*
Compilación sugerida:
gcc -Wall -I../include counter_lock.c benchmark_precise_counter.c -lpthread -o benchmark_precise_counter
*/

// Cada hilo incrementará 1,000,000 de veces
#define COUNT_PER_THREAD 1000000

// El contador global
counter_t cnt;

// Prototipo de la función que ejecutan los hilos
void *counting(void *arg);

int main() {
    FILE *fp = fopen("results_lock.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results_lock.csv para escritura.\n");
        exit(1);
    }

    // Escribir el encabezado del CSV
    fprintf(fp, "Threads,Time(s),Expected,Actual\n");
    printf("Iniciando benchmark para 'counter_lock'...\n");
    printf("Resultados se guardarán en results_lock.csv\n");

    // Probaremos con 1, 2, 4, 8 y 16 hilos
    const int thread_counts[] = {1, 2, 4, 8, 16};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    for (int t = 0; t < num_tests; t++) {
        int num_threads = thread_counts[t];
        long expected_value = (long)num_threads * COUNT_PER_THREAD;
        
        printf("  Probando con %d hilos...\n", num_threads);

        // Reservar memoria para los IDs de los hilos
        pthread_t *tid = malloc(sizeof(pthread_t) * num_threads);
        if (tid == NULL) {
            fprintf(stderr, "Error: No se pudo reservar memoria para los hilos.\n");
            continue; // Saltar esta prueba
        }

        // --- Ejecución de la prueba ---
        init(&cnt); // Reiniciar el contador
        
        double t_ini = GetTime(); // Marca de tiempo inicial
        
        // Creamos los hilos
        for (int i = 0; i < num_threads; i++) {
            // Pasamos el contador global a cada hilo
            pthread_create(&tid[i], NULL, counting, &cnt);
        }
        
        // Esperamos a que terminen (Join)
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        
        double t_end = GetTime(); // Marca de tiempo final
        
        // --- Recolección de resultados ---
        double time_taken = t_end - t_ini;
        int actual_value = get(&cnt);

        // Imprimir a consola
        printf("    Tiempo: %.4f s, Esperado: %ld, Real: %d\n", 
               time_taken, expected_value, actual_value);
        
        // Escribir al archivo CSV
        fprintf(fp, "%d,%.4f,%ld,%d\n", 
                num_threads, time_taken, expected_value, actual_value);

        free(tid); // Liberar memoria
    }

    fclose(fp);
    printf("Benchmark completado.\n");

    return 0;
}

/**
 * @brief Definición de la función del hilo
 * Esta función es idéntica a la de tu precise_counter.c
 */
void *counting(void *cnt_t) { 
  counter_t *r_cnt = (counter_t *)cnt_t;
  for (int i = 0; i < COUNT_PER_THREAD; i++) {
    increment(r_cnt);
  } 
  return NULL; 
}