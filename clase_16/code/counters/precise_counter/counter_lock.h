#ifndef __counter_lock_h__
#define __counter_lock_h__

#include <stddef.h>   // Define NULL
#include <pthread.h>

typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c);
void increment(counter_t *c);
void decrement(counter_t *c);
int get(counter_t *c);

#endif // __counter_lock_h__