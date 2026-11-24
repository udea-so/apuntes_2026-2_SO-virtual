#ifndef __zemaphore_h__
#define __zemaphore_h__

/*
 * zemaphore.h - Semaforo ("Zemaforo") construido con un lock y una variable
 * de condicion.
 *
 * Diapositivas (parte 2) p. 47-48; OSTEP cap. 31, §31.8, Fig. 31.17
 * (Implementing Zemaphores With Locks And CVs).
 *
 * A diferencia del semaforo de Dijkstra (p. 5 y Fig. 31.2), el valor nunca
 * baja de 0: un valor negativo no cuenta los hilos que esperan. Es el mismo
 * comportamiento de los semaforos de Linux.
 */

#include <pthread.h>
#include "common_threads.h"

typedef struct __Zem_t {
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Zem_t;

// only one thread can call this
void Zem_init(Zem_t *s, int value) {
    s->value = value;
    Cond_init(&s->cond);
    Mutex_init(&s->lock);
}

void Zem_wait(Zem_t *s) {
    Mutex_lock(&s->lock);
    while (s->value <= 0)
        Cond_wait(&s->cond, &s->lock);
    s->value--;
    Mutex_unlock(&s->lock);
}

void Zem_post(Zem_t *s) {
    Mutex_lock(&s->lock);
    s->value++;
    Cond_signal(&s->cond);
    Mutex_unlock(&s->lock);
}

// No esta en OSTEP: lee el valor (con el lock) para poder mostrarlo.
int Zem_getvalue(Zem_t *s) {
    Mutex_lock(&s->lock);
    int v = s->value;
    Mutex_unlock(&s->lock);
    return v;
}

#endif // __zemaphore_h__
