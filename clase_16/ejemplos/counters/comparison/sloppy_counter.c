#include "sloppy_counter.h"

// Mismas operaciones de ../sloppy_counter/sloppy_counter.c; solo cambia
// c->local[i] por c->local[i].value y c->llock[i] por c->local[i].lock.

void sloppy_init(sloppy_counter_t *c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    for (int i = 0; i < NUMCPUS; i++) {
        c->local[i].value = 0;
        pthread_mutex_init(&c->local[i].lock, NULL);
    }
}

void sloppy_update(sloppy_counter_t *c, int threadID, int amt) {
    local_counter_t *l = &c->local[threadID];
    pthread_mutex_lock(&l->lock);
    l->value += amt;                    // assumes amt > 0
    if (l->value >= c->threshold) {     // transfer to global
        pthread_mutex_lock(&c->glock);
        c->global += l->value;
        pthread_mutex_unlock(&c->glock);
        l->value = 0;
    }
    pthread_mutex_unlock(&l->lock);
}

int sloppy_get(sloppy_counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
}
