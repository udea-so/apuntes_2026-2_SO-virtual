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
gcc -Wall -I../include sloppy_counter.c benchmark_sloppy_counter.c -lpthread -o benchmark_sloppy
*/

// Cada hilo incrementará 1,000,000 de veces
#define COUNT_PER_THREAD 1000000

#define THRESHOLD 1000000 // Umbral para la actualización global
#define AMT 1          // Incremento por llamada

// El contador global
counter_t cnt;

// --- ESTRUCTURA DE ARGUMENTOS ---
// Necesaria para pasar el threadID a cada hilo
typedef struct {
    counter_t *c;
    int max_value;
    int threadID;
    int amt;
} args_t;


// Prototipo de la función que ejecutan los hilos
void *counting(void *arg);

int main() {
    FILE *fp = fopen("results_sloppy.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results_sloppy.csv para escritura.\n");
        exit(1);
    }

    // Escribir el encabezado del CSV
    fprintf(fp, "Threads,Time(s),Expected,Actual\n");
    printf("Iniciando benchmark para 'sloppy_counter' (Threshold=%d)...\n", THRESHOLD);
    printf("Resultados se guardarán en results_sloppy.csv\n");

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
        // --- CAMBIO: Llamar a init con el THRESHOLD ---
        init(&cnt, THRESHOLD); 
        
        double t_ini = GetTime(); // Marca de tiempo inicial
        
        // --- CAMBIO: Bucle de creación de hilos ---
        for (int i = 0; i < num_threads; i++) {
            
            // 1. Asignar memoria para los argumentos de ESTE hilo
            args_t *args = malloc(sizeof(args_t));
            if (args == NULL) {
                perror("malloc args");
                exit(1);
            }

            // 2. Poblar los argumentos
            args->c = &cnt;
            args->max_value = COUNT_PER_THREAD;
            args->amt = AMT;
            // 3. Asignar ID de CPU/lock local usando módulo
            args->threadID = i % NUMCPUS; 
            
            // 4. Crear el hilo con sus propios argumentos
            pthread_create(&tid[i], NULL, counting, args);
        }
        
        // Esperamos a que terminen (Join)
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        
        double t_end = GetTime(); // Marca de tiempo final
        
        // --- Recolección de resultados ---
        double time_taken = t_end - t_ini;
        // --- CAMBIO: Usar get_precise para el valor real ---
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
 * Adaptada para 'sloppy_counter'
 */
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
