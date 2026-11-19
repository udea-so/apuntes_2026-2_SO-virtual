/*
 * my_cv.c - Señalización entre hilos con un semáforo (ver my_cv.h).
 */
#include "my_cv.h"
#include "common_threads.h"

void my_cv_init(my_cv *cv) {
    // Inicializamos en 0 para bloquear al primer hilo que llame a wait
    Sem_init(&cv->sem, 0);
}

void my_cv_wait(my_cv *cv) {
    // Si el valor es 0, bloquea al hilo hasta que alguien haga post;
    // si es mayor que 0 (ya hubo un post), lo decrementa y continúa
    Sem_wait(&cv->sem);
}

void my_cv_signal(my_cv *cv) {
    // Incrementa el semáforo y, si hay un hilo esperando, lo despierta
    Sem_post(&cv->sem);
}
