#ifndef __sloppy_counter_h__
#define __sloppy_counter_h__

#include <stddef.h>   // NULL
#include <pthread.h>

// Contador aproximado SIN false sharing.
//
// La logica es la misma de ../sloppy_counter/ (diapositivas p. 20-23); lo
// unico que cambia es la distribucion en memoria. En la version de las
// diapositivas, local[NUMCPUS] (8 int = 32 bytes) cabe en una sola linea de
// cache, de modo que los hilos que actualizan contadores locales distintos
// se invalidan la linea entre si en cada incremento (false sharing). Aqui
// cada contador local va junto con su lock en un struct alineado a 64 bytes,
// asi que cada uno ocupa su propia linea de cache.

#define NUMCPUS 8              // CPU logicas de la maquina de referencia (nproc)
#define CACHE_LINE_SIZE 64     // tamano de linea de cache en x86-64

typedef struct __local_counter_t {
    int value;                 // local count
    pthread_mutex_t lock;      // local lock
} __attribute__((aligned(CACHE_LINE_SIZE))) local_counter_t;

typedef struct __sloppy_counter_t {
    int global;                       // global count
    pthread_mutex_t glock;            // global lock
    int threshold;                    // update frequency
    local_counter_t local[NUMCPUS];   // local counts and locks (one per cache line)
} sloppy_counter_t;

void sloppy_init(sloppy_counter_t *c, int threshold);
void sloppy_update(sloppy_counter_t *c, int threadID, int amt);
int sloppy_get(sloppy_counter_t *c);

#endif // __sloppy_counter_h__
