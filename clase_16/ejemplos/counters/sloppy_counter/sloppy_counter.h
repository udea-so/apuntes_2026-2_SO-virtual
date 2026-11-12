#ifndef __sloppy_counter_h__
#define __sloppy_counter_h__

#include <stddef.h>   // NULL
#include <pthread.h>

// Numero de contadores locales. Se fija en el numero de CPU logicas de la
// maquina de referencia (AMD Ryzen 5 3500U: 4 nucleos fisicos, 8 logicos).
// Para consultarlo en otra maquina: nproc
#define NUMCPUS 8

typedef struct __counter_t {
  int global;                       // global count
  pthread_mutex_t glock;            // global lock
  int local[NUMCPUS];               // local count (per cpu)
  pthread_mutex_t llock[NUMCPUS];   // ... and locks
  int threshold;                    // update frequency
} counter_t;

void init(counter_t *c, int threshold);
void update(counter_t *c, int threadID, int amt);
int get(counter_t *c);

#endif // __sloppy_counter_h__
