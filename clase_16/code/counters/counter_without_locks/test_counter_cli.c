#include <stdio.h>
#include <stdlib.h>     // Para atoi, malloc, free, exit
#include <stdint.h>     // Para intptr_t (paso de argumentos)
#include <pthread.h>
#include "common.h"
#include "common_threads.h"
#include "counter.h"    

/*
Compilación sugerida:
gcc -Wall -I../include test_counter_cli.c counter.c -lpthread -o counter_test
*/

// El contador global
counter_t cnt;

void *counting(void *arg);


int main(int argc, char *argv[]) {
    
    // --- 1. Validar Argumentos ---
    if (argc != 3) {
        // Imprime a stderr (salida de error estándar)
        fprintf(stderr, "Uso: %s <num_threads> <count_per_thread>\n", argv[0]);
        exit(1); // Sale con código de error
    }

    // --- 2. Parsear Argumentos ---
    int num_threads = atoi(argv[1]);
    int count_per_thread = atoi(argv[2]);

    // Validación de argumentos positivos
    if (num_threads <= 0 || count_per_thread <= 0) {
        fprintf(stderr, "Error: Los argumentos deben ser números positivos.\n");
        exit(1);
    }

    // --- 3. Inicialización ---
    init(&cnt); // Inicializa el contador y su mutex
    
    int i;
    double t_ini, t_end;
    
    // Se reserva memoria para los IDs de los hilos dinámicamente
    pthread_t *tid = malloc(sizeof(pthread_t) * num_threads);
    if (tid == NULL) {
        fprintf(stderr, "Error: No se pudo reservar memoria para los hilos.\n");
        exit(1);
    }

    printf("Iniciando prueba con %d hilos, %d incrementos cada uno.\n", 
           num_threads, count_per_thread);

    // --- 4. Creación de Hilos ---
    t_ini = GetTime(); // Marca de tiempo inicial
    
    for (i = 0; i < num_threads; i++) {
        // Se pasa 'count_per_thread' como argumento al hilo.
        // Se emplea intptr_t para convertir de forma segura el entero a un
        // tipo "del tamaño de un puntero" y luego a void*.     
        void *arg = (void *)(intptr_t)count_per_thread;
        pthread_create(&tid[i], NULL, counting, arg);
    }
    
    // --- 5. Espera (Join) de Hilos ---
    for (i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
    }
    
    t_end = GetTime(); // Marca de tiempo final
    
    // --- 6. Mostrar Resultados ---
    printf("\n-> Tiempo gastado: %lf seg\n", t_end - t_ini);
    
    // Usamos 'long' para el valor esperado, en caso de que
    // el resultado sea mayor a 2^31 (overflow de 'int')
    long expected_value = (long)num_threads * count_per_thread;
    printf("-> El contador debe quedar en: %ld\n", expected_value);
    
    // Llama a la función 'get' de counter.c
    printf("-> El valor real del contador es: %d\n", get(&cnt));

    // --- 7. Limpieza ---
    free(tid); // Liberamos la memoria de los IDs de hilo
    
    return 0;
}

// Definición de la función del hilo
void *counting(void *arg) { 
    
    // Convertimos el argumento (void*) de vuelta a un entero.
    int limit = (int)(intptr_t)arg;
    
    for(int i = 0; i < limit; i++) {
        // Llama a la función 'increment' segura
        increment(&cnt); 
    } 
    return NULL; 
}