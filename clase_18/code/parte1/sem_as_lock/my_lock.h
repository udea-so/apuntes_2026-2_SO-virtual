#ifndef __MY_LOCK_H__
#define __MY_LOCK_H__

#include <semaphore.h>

typedef struct __lock_t {
    sem_t sem;
} lock_t;

void init(lock_t *lock);
void acquire(lock_t *lock);
void release(lock_t *lock);

#endif // __MY_LOCK_H__