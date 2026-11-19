#include "my_lock.h"
#include "common_threads.h" 

void init(lock_t *lock) {
    // Usamos el wrapper Sem_init. 
    // El segundo argumento '1' indica que inicia desbloqueado (comportamiento Mutex).
    Sem_init(&lock->sem, 1); 
}

void acquire(lock_t *lock) {
    // Usamos el wrapper Sem_wait (equivale a lock/P)
    Sem_wait(&lock->sem);
}

void release(lock_t *lock) {
    // Usamos el wrapper Sem_post (equivale a unlock/V)
    Sem_post(&lock->sem);
}