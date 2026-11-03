#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

// Asumimos que common.h está en ../include y tiene GetTime()
#include "common.h" 

// Incluimos AMBOS contadores (asegúrate de renombrar las funciones)
#include "precise_counter.h"
#include "sloppy_counter.h"

/*
Compilación (después de renombrar funciones):
gcc -Wall -I../include precise_counter.c sloppy_counter.c benchmark_counters.c -lpthread -o run_benchmark
*/

// --- CONFIGURACIÓN DEL BENCHMARK ---

// Hilos a probar (para replicar tu gráfica)
const int THREAD_COUNTS[] = {1, 2, 4, 8, 16, 32};
#define NUM_TESTS (sizeof(THREAD_COUNTS) / sizeof(THREAD_COUNTS[0]))

// Conteo por hilo (ajusta esto para que dure segundos, como en la gráfica)
#define COUNT_PER_THREAD 10000000 

// Configuración del contador Sloppy
#define THRESHOLD 1024
#define AMT 1

// --- Contadores Globales ---
precise_counter_t precise_cnt;
sloppy_counter_t sloppy_cnt;

// --- Argumentos para el hilo Sloppy ---
typedef struct {
    int threadID;
    int amt;
    int max_value;
} sloppy_args_t;

// --- Prototipos de funciones ---
void *worker_precise(void *arg);
void *worker_sloppy(void *arg);
int get_precise_sloppy(sloppy_counter_t *c);

// --- MAIN ---
int main() {
    FILE *fp = fopen("results.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir results.csv\n");
        exit(1);
    }
    
    // Escribir encabezado del CSV
    fprintf(fp, "Threads,CounterType,Time(s)\n");
    printf("Iniciando benchmark... (Conteo por hilo: %d)\n", COUNT_PER_THREAD);

    // Iterar sobre las cantidades de hilos (1, 2, 3, 4)
    for (int t_idx = 0; t_idx < NUM_TESTS; t_idx++) {
        int num_threads = THREAD_COUNTS[t_idx];
        printf("\nProbando con %d hilos...\n", num_threads);

        pthread_t *tid = malloc(sizeof(pthread_t) * num_threads);
        double t_ini, t_end, time_taken;

        // --- 1. PRUEBA DEL CONTADOR PRECISO ---
        // (Asume que has renombrado a 'precise_init', etc.)
        precise_init(&precise_cnt);
        
        t_ini = GetTime();
        for (int i = 0; i < num_threads; i++) {
            pthread_create(&tid[i], NULL, worker_precise, NULL);
        }
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        t_end = GetTime();
        time_taken = t_end - t_ini;
        
        printf("  Precise: %.4f segundos\n", time_taken);
        fprintf(fp, "%d,Precise,%.4f\n", num_threads, time_taken);
        // (Opcional) Verificar el conteo:
        // long precise_val = precise_get(&precise_cnt);
        // printf("  Precise Val: %ld\n", precise_val);

        // --- 2. PRUEBA DEL CONTADOR APROXIMADO (SLOPPY) ---
        // (Asume que has renombrado a 'sloppy_init', etc.)
        sloppy_init(&sloppy_cnt, THRESHOLD);
        
        t_ini = GetTime();
        for (int i = 0; i < num_threads; i++) {
            // Asignar argumentos para cada hilo
            sloppy_args_t *args = malloc(sizeof(sloppy_args_t));
            args->max_value = COUNT_PER_THREAD;
            args->amt = AMT;
            // Usar módulo para asignar ID de lock local (evita desbordamiento)
            args->threadID = i % NUMCPUS; 
            
            pthread_create(&tid[i], NULL, worker_sloppy, args);
        }
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        t_end = GetTime();
        time_taken = t_end - t_ini;

        printf("  Approximate: %.4f segundos\n", time_taken);
        fprintf(fp, "%d,Approximate,%.4f\n", num_threads, time_taken);
        // (Opcional) Verificar el conteo usando el 'get' preciso:
        // long sloppy_val = get_precise_sloppy(&sloppy_cnt);
        // printf("  Sloppy Val: %ld\n", sloppy_val);

        free(tid);
    }

    fclose(fp);
    printf("\nBenchmark completado. Resultados guardados en results.csv\n");
    return 0;
}

// --- Hilo de trabajo para el contador PRECISO ---
void *worker_precise(void *arg) {
    for (int i = 0; i < COUNT_PER_THREAD; i++) {
        precise_increment(&precise_cnt); // Llama a la función renombrada
    }
    return NULL;
}

// --- Hilo de trabajo para el contador APROXIMADO (SLOPPY) ---
void *worker_sloppy(void *arg) {
    sloppy_args_t *rvals = (sloppy_args_t *)arg;
    
    for (int i = 0; i < rvals->max_value; i++) {
        // Llama a la función renombrada
        sloppy_update(&sloppy_cnt, rvals->threadID, rvals->amt);
    }
    
    free(rvals); // Liberar argumentos
    return NULL;
}

/**
 * @brief Obtiene el valor preciso del contador sloppy.
 * Útil para verificar el conteo final después de que los hilos terminan.
 */
int get_precise_sloppy(sloppy_counter_t *c) {
    int val = 0;
    // Bloquear el global primero
    pthread_mutex_lock(&c->glock);
    val = c->global;
    pthread_mutex_unlock(&c->glock);

    // Sumar todos los locales
    for (int i = 0; i < NUMCPUS; i++) {
        pthread_mutex_lock(&c->llock[i].lock);
        val += c->local[i].value;
        pthread_mutex_unlock(&c->llock[i].lock);
    }
    return val;
}