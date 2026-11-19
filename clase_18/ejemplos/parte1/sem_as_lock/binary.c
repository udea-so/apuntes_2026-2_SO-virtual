/*
 * binary.c - Contador compartido protegido con el lock_t implementado con
 *            un semáforo en my_lock.c (diapositivas p. 18-19).
 *
 * Es el mismo programa de diapositivas/binary_sem/binary.c, pero en lugar
 * de llamar directamente a sem_wait()/sem_post() usa las funciones
 * init(), acquire() y release() de la interfaz de lock de la p. 19.
 *
 * Uso:         ./binary.out
 */
#include <stdio.h>
#include <stdlib.h>
#include "common.h"         // Para GetTime, Spin (si se necesitan)
#include "common_threads.h" // Para Pthread_create, Pthread_join
#include "my_lock.h"        // Nuestra implementación del mutex

// Variable compartida
volatile int counter = 0;

// Instancia de nuestro lock
lock_t mutex;

void *child(void *arg) {
    int i;
    for (i = 0; i < 10000000; i++) {
        // Entrada a Sección Crítica
        acquire(&mutex);

        counter++;

        // Salida de Sección Crítica
        release(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // 1. Inicializamos el lock usando nuestra librería
    init(&mutex);

    pthread_t c1, c2;

    // 2. Creamos los hilos usando los wrappers de common_threads.h
    Pthread_create(&c1, NULL, child, NULL);
    Pthread_create(&c2, NULL, child, NULL);

    // 3. Esperamos a los hilos
    Pthread_join(c1, NULL);
    Pthread_join(c2, NULL);

    printf("result: %d (should be 20000000)\n", counter);

    return 0;
}
