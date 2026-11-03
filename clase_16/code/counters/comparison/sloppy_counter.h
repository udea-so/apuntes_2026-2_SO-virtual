#ifndef __sloppy_counter_h__
#define __sloppy_counter_h__

#include <pthread.h>

#define NUMCPUS 8  // Numero de CPUS: cat /proc/cpuinfo | grep 'processor' | wc -l
                   // La CPU AMD Ryzen 5 3500U tiene 4 cores fisicos y 8 hilos (threads): NUMCPUS = 8 
#define CACHE_LINE_SIZE 64 // Tamaño típico de línea de caché en bytes

// Nueva estructura para un contador local RELLENO
typedef struct __local_counter_t {
    int value;
    char padding[CACHE_LINE_SIZE - sizeof(int)]; // Relleno para llegar a 64 bytes
} local_counter_t;

// --- NUEVO ---
// Estructura de lock local RELLENA
typedef struct __local_lock_t {
    pthread_mutex_t lock;
    char padding[CACHE_LINE_SIZE - sizeof(pthread_mutex_t)];
} local_lock_t;
// --- FIN NUEVO ---

typedef struct __sloppy_counter_t {
  int global;                       // global count
  pthread_mutex_t glock;            // global lock
  local_counter_t local[NUMCPUS];   // local count (per cpu)
  local_lock_t llock[NUMCPUS];   // ... and locks
  int threshold;                    // update frequency
} sloppy_counter_t;

// init: record threshold, init locks, init values
//       of all local counts and global count
void sloppy_init(sloppy_counter_t *c, int threshold);

// update: usually, just grab local lock and update local amount
//         once local count has risen by 'threshold', grab global
//         lock and transfer local values to it
void sloppy_update(sloppy_counter_t *c, int threadID, int amt);


// get: just return global amount (which may not be perfect)
int sloppy_get(sloppy_counter_t *c);

#endif // __sloppy_counter_h__