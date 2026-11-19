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

    // 2. Creamos los hilos usando tus wrappers
    Pthread_create(&c1, NULL, child, NULL);
    Pthread_create(&c2, NULL, child, NULL);

    // 3. Esperamos a los hilos
    Pthread_join(c1, NULL);
    Pthread_join(c2, NULL);

    printf("result: %d (should be 20000000)\n", counter);
    
    return 0;
}