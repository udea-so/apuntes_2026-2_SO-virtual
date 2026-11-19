/*
 * my_lock.c - Lock implementado con un semáforo (diapositiva p. 19).
 */
#include "my_lock.h"
#include "common_threads.h"

void init(lock_t *lock) {
    // Sem_init(&sem, valor) es sem_init(&sem, 0, valor): el semáforo se
    // comparte entre los hilos del proceso. El valor 1 indica que el lock
    // empieza libre.
    Sem_init(&lock->sem, 1);
}

void acquire(lock_t *lock) {
    // Sem_wait equivale a lock (P o down)
    Sem_wait(&lock->sem);
}

void release(lock_t *lock) {
    // Sem_post equivale a unlock (V o up)
    Sem_post(&lock->sem);
}
