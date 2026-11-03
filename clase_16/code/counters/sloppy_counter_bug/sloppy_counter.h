#ifndef __sloppy_counter_h__
#define __sloppy_counter_h__

#include <pthread.h>
#include <stdio.h>

#define NUMCPUS 8  // Numero de CPUS: cat /proc/cpuinfo | grep 'processor' | wc -l
                   // La CPU AMD Ryzen 5 3500U tiene 4 cores fisicos y 8 hilos (threads): NUMCPUS = 8 

typedef struct __counter_t {
  int global;                       // global count
  pthread_mutex_t glock;            // global lock
  int local[NUMCPUS];               // local count (per cpu)
  pthread_mutex_t llock[NUMCPUS];   // ... and locks
  int threshold;                    // update frequency
} counter_t;

// init: record threshold, init locks, init values
//       of all local counts and global count
void init(counter_t *c, int threshold);

// update: usually, just grab local lock and update local amount
//         once local count has risen by 'threshold', grab global
//         lock and transfer local values to it
void update(counter_t *c, int threadID, int amt);


// get: just return global amount (which may not be perfect)
int get(counter_t *c);

#endif // __sloppy_counter_h__