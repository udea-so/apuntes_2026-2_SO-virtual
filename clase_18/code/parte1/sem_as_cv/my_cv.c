#include "my_cv.h"
#include "common_threads.h" //

void my_cv_init(my_cv *cv) {
    // Inicializamos en 0 para bloquear al primer hilo que llame a wait
    Sem_init(&cv->sem, 0);
}

void my_cv_wait(my_cv *cv) {
    // Bloquea el hilo hasta que reciba la señal (sem > 0)
    Sem_wait(&cv->sem);
}

void my_cv_signal(my_cv *cv) {
    // Incrementa el semáforo para despertar al hilo en espera
    Sem_post(&cv->sem);
}