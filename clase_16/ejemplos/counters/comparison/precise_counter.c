#include "precise_counter.h"

// Mismo contador de ../precise_counter/counter_lock.c, con las funciones
// renombradas (precise_*) para poder enlazarlo junto al contador aproximado.

void precise_init(precise_counter_t *c) {
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void precise_increment(precise_counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void precise_decrement(precise_counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int precise_get(precise_counter_t *c) {
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}
