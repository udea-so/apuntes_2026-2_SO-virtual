#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h> // Necesario para gettimeofday
#include <assert.h>   // Necesario para assert
#include "common.h" // Para GetTime() y Spin()


// ======================================================
// Estructura y función del hilo (sin cambios)
// ======================================================
typedef struct {
    int id;
    int* vector1;
    int* vector2;
    int* vectorSuma;
    int tamano_total;
    int num_hilos;
} thread_args_t;

void* sumar_porcion_vector(void* args) {
    thread_args_t* data = (thread_args_t*)args;
    int tamano_trozo = data->tamano_total / data->num_hilos;
    int inicio = data->id * tamano_trozo;
    int fin = inicio + tamano_trozo;

    if (data->id == data->num_hilos - 1) {
        fin = data->tamano_total;
    }

    for (int i = inicio; i < fin; i++) {
        data->vectorSuma[i] = data->vector1[i] + data->vector2[i];
        // Descomenta la siguiente línea para simular una carga de trabajo más pesada
        // Spin(0.000001);
    }

    return NULL;
}

// ======================================================
// Función principal con tus funciones integradas
// ======================================================
int main(int argc, char *argv[]) {
    // Debe ser el mismo valor que en suma_s.c para que Ts y Tp sean comparables.
    const int TAMANO = 100000000; // 100M → ~1.2 GB. Cambie a 1000000000 si tiene 16+ GB libres.
    int* v1 = (int*)malloc(TAMANO * sizeof(int));
    int* v2 = (int*)malloc(TAMANO * sizeof(int));
    int* resultado = (int*)malloc(TAMANO * sizeof(int));

    if (!v1 || !v2 || !resultado) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    // Inicializar vectores. También se escribe en resultado para que sus
    // fallos de página ocurran aquí y no dentro de la medición.
    for (int i = 0; i < TAMANO; i++) {
        v1[i] = i;
        v2[i] = i * 2;
        resultado[i] = 0;
    }

    // Por defecto, un hilo por núcleo lógico; opcionalmente: ./suma_p <num_hilos>
    const int NUM_HILOS = (argc > 1) ? atoi(argv[1]) : (int) sysconf(_SC_NPROCESSORS_ONLN);
    if (NUM_HILOS < 1) {
        fprintf(stderr, "uso: %s [num_hilos >= 1]\n", argv[0]);
        return 1;
    }
    printf("Usando %d hilos para la suma.\n", NUM_HILOS);

    pthread_t hilos[NUM_HILOS];
    thread_args_t args_hilo[NUM_HILOS];

    // INICIA LA MEDICIÓN DE TIEMPO
    double t_inicio = GetTime();

    // Crear los hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        args_hilo[i] = (thread_args_t){i, v1, v2, resultado, TAMANO, NUM_HILOS};
        pthread_create(&hilos[i], NULL, sumar_porcion_vector, (void*)&args_hilo[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    // FINALIZA LA MEDICIÓN DE TIEMPO
    double t_fin = GetTime();
    double tiempo_total = t_fin - t_inicio;

    printf("Suma paralela completada.\n");
    printf("Tiempo de ejecución paralela (Tp): %f segundos.\n", tiempo_total);

    // Liberar memoria
    free(v1);
    free(v2);
    free(resultado);

    return 0;
}