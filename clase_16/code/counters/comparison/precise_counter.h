#ifndef __precise_counter_h__
#define __precise_counter_h__

#include <stddef.h>   // Define NULL
#include <pthread.h>

typedef struct __precise_counter_t {
    int value;
    pthread_mutex_t lock;
} precise_counter_t;

void precise_init(precise_counter_t *c);
void precise_increment(precise_counter_t *c);
void precise_decrement(precise_counter_t *c);
int precise_get(precise_counter_t *c);

#endif // __precise_counter_h__