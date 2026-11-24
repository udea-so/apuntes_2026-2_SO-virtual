/*
 * rwlock.c - Lock de lectores-escritores (reader-writer lock) con semaforos.
 *
 * Diapositivas (parte 2) p. 26-34; OSTEP cap. 31, §31.5, Fig. 31.13
 * (A Simple Reader-Writer Lock).
 *
 * Compilacion: gcc -Wall -I. -o rwlock.out rwlock.c -pthread
 * Uso:         ./rwlock.out <read_loops> <write_loops> [lectores] [escritores] [espera_ms]
 *
 *   Cada lector lee el contador compartido <read_loops> veces y cada escritor
 *   lo incrementa <write_loops> veces. Por defecto hay 1 lector y 1 escritor.
 *   espera_ms (opcional, por defecto 0): milisegundos que cada escritor espera
 *   antes de empezar, para que llegue cuando los lectores ya estan leyendo.
 *
 *   Para comprobar el lock, el programa cuenta (con operaciones atomicas, que
 *   no son parte del lock) cuantos hilos estan dentro de la seccion critica en
 *   cada momento: al final informa el maximo de lectores simultaneos y si
 *   alguna vez un escritor estuvo acompaniado (violacion de la exclusion).
 *   Cada hilo informa tambien el instante en que termino, lo que permite ver
 *   si los escritores tuvieron que esperar a los lectores (inanicion, p. 34).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"

#define RMAX 16
#define WMAX 16

// Variante -DLECTURA_LARGA (rwlock-lectura-larga.out): cada lectura dura mas
// (un ciclo vacio dentro de la seccion critica), de modo que los lectores se
// solapan y readers casi nunca vuelve a 0: el escritor espera (inanicion, p. 34).
#ifdef LECTURA_LARGA
#define LEER_DESPACIO() { volatile int k; for (k = 0; k < 10000; k++) ; }
#else
#define LEER_DESPACIO()
#endif

/* ------------------------ Reader-writer lock (Fig. 31.13) ----------------------- */

typedef struct _rwlock_t {
    sem_t lock;        // binary semaphore (basic lock): protege readers
    sem_t writelock;   // allow ONE writer/MANY readers
    int   readers;     // #readers in critical section
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    Sem_init(&rw->lock, 1);        // sem_init(&rw->lock, 0, 1)
    Sem_init(&rw->writelock, 1);   // sem_init(&rw->writelock, 0, 1)
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    Sem_wait(&rw->lock);
    rw->readers++;
    if (rw->readers == 1)          // first reader gets writelock
        Sem_wait(&rw->writelock);
    Sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw) {
    Sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)          // last reader lets it go
        Sem_post(&rw->writelock);
    Sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    Sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t *rw) {
    Sem_post(&rw->writelock);
}

/* ------------------------------ Prueba ------------------------------------ */

int read_loops;
int write_loops;
int espera_ms = 0;
int counter = 0;          // dato compartido
rwlock_t rw;
double t0;                // inicio de la prueba (GetTime() de common.h)

// Instrumentacion (atomica, fuera del lock): hilos dentro de la seccion critica.
int lectores_dentro   = 0;
int escritores_dentro = 0;
int max_lectores      = 0;
int violaciones       = 0;

void entra_lector(void) {
    int n = __atomic_add_fetch(&lectores_dentro, 1, __ATOMIC_SEQ_CST);
    int m = __atomic_load_n(&max_lectores, __ATOMIC_SEQ_CST);
    while (n > m && !__atomic_compare_exchange_n(&max_lectores, &m, n, 0,
                                                  __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
        ;
    if (__atomic_load_n(&escritores_dentro, __ATOMIC_SEQ_CST) != 0)
        __atomic_add_fetch(&violaciones, 1, __ATOMIC_SEQ_CST);
}

void sale_lector(void) {
    __atomic_sub_fetch(&lectores_dentro, 1, __ATOMIC_SEQ_CST);
}

void entra_escritor(void) {
    int w = __atomic_add_fetch(&escritores_dentro, 1, __ATOMIC_SEQ_CST);
    if (w != 1 || __atomic_load_n(&lectores_dentro, __ATOMIC_SEQ_CST) != 0)
        __atomic_add_fetch(&violaciones, 1, __ATOMIC_SEQ_CST);
}

void sale_escritor(void) {
    __atomic_sub_fetch(&escritores_dentro, 1, __ATOMIC_SEQ_CST);
}

void *reader(void *arg) {
    int id = *(int *) arg;
    int i, local = 0;
    for (i = 0; i < read_loops; i++) {
        rwlock_acquire_readlock(&rw);
        entra_lector();
        local = counter;                 // lectura
        LEER_DESPACIO();
        sale_lector();
        rwlock_release_readlock(&rw);
    }
    printf("R%d: lectura terminada en t = %.3f s, ultimo valor leido %d\n", id, GetTime() - t0, local);
    return NULL;
}

void *writer(void *arg) {
    int id = *(int *) arg;
    int i;
    if (espera_ms > 0)
        usleep(espera_ms * 1000);   // llega cuando los lectores ya estan leyendo
    for (i = 0; i < write_loops; i++) {
        rwlock_acquire_writelock(&rw);
        entra_escritor();
        counter++;                       // escritura
        sale_escritor();
        rwlock_release_writelock(&rw);
    }
    printf("W%d: escritura terminada en t = %.3f s\n", id, GetTime() - t0);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 6) {
        fprintf(stderr, "uso: %s <read_loops> <write_loops> [lectores] [escritores] [espera_ms]\n", argv[0]);
        exit(1);
    }
    read_loops  = atoi(argv[1]);
    write_loops = atoi(argv[2]);
    int lectores   = (argc > 3) ? atoi(argv[3]) : 1;
    int escritores = (argc > 4) ? atoi(argv[4]) : 1;
    if (argc > 5)
        espera_ms = atoi(argv[5]);
    assert(lectores >= 0 && lectores <= RMAX);
    assert(escritores >= 0 && escritores <= WMAX);

    rwlock_init(&rw);
    t0 = GetTime();

    // Primero se crean los lectores y luego los escritores: cuando un escritor
    // llega, los lectores ya estan leyendo (escenario de la p. 34).
    pthread_t r[RMAX], w[WMAX];
    int rid[RMAX], wid[WMAX];
    int i;
    for (i = 0; i < lectores; i++) {
        rid[i] = i + 1;
        Pthread_create(&r[i], NULL, reader, &rid[i]);
    }
    for (i = 0; i < escritores; i++) {
        wid[i] = i + 1;
        Pthread_create(&w[i], NULL, writer, &wid[i]);
    }
    for (i = 0; i < lectores; i++)
        Pthread_join(r[i], NULL);
    for (i = 0; i < escritores; i++)
        Pthread_join(w[i], NULL);

    printf("contador final: %d (esperado %d)\n", counter, escritores * write_loops);
    printf("lectores simultaneos (maximo): %d\n", max_lectores);
    printf("violaciones de la exclusion: %d\n", violaciones);
    return 0;
}
