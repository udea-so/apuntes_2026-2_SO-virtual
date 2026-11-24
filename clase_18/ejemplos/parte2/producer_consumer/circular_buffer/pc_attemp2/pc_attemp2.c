/*
 * pc_attemp2.c - Productor/consumidor con buffer circular, empty, full y un
 * semaforo binario mutex que envuelve TODO el cuerpo del ciclo, incluido el
 * sem_wait sobre empty/full. Solucion ROTA: deadlock.
 *
 * Diapositivas (parte 2) p. 17-21; OSTEP cap. 31, Fig. 31.11
 * (Adding Mutual Exclusion (Incorrectly)).
 *
 * Compilacion: gcc -Wall -I. -o pc_attemp2.out pc_attemp2.c -pthread
 * Con traza:   gcc -Wall -I. -DTRAZA -o pc_attemp2-traza.out pc_attemp2.c -pthread
 * Uso:         ./pc_attemp2.out <buffersize> <loops> <productores> <consumidores> [espera_ms]
 *
 *   espera_ms (opcional, por defecto 0): milisegundos que cada productor espera
 *   antes de empezar. Hace que el consumidor corra primero, con el buffer
 *   vacio: es el escenario de las p. 19-20 y de OSTEP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"
#include "verifica.h"

// Traza opcional (compilar con -DTRAZA): cada hilo imprime en que linea esta
// antes de cada sem_wait, con las etiquetas P0-P4 / C0-C4 de las diapositivas.
#ifdef TRAZA
#define TRAZA_PRINTF(...) printf(__VA_ARGS__)
#else
#define TRAZA_PRINTF(...)
#endif

int max;                  // MAX en las diapositivas: tamanio del buffer
int loops;
int producers;
int consumers;
int espera_ms = 0;

/* ---------------------- Buffer circular (p. 10) --------------------------- */

int *buffer;
int fill = 0;
int use  = 0;

void put(int value) {
    buffer[fill] = value;          // Line F1
    fill = (fill + 1) % max;       // Line F2
}

int get(void) {
    int tmp = buffer[use];         // Line G1
    use = (use + 1) % max;         // Line G2
    return tmp;
}

/* ------------------------------ Semaforos --------------------------------- */

sem_t empty;              // espacios vacios: empieza en MAX
sem_t full;               // espacios llenos: empieza en 0
sem_t mutex;              // semaforo binario: empieza en 1

/* ------------------------- Productor y consumidor ------------------------- */

void *producer(void *arg) {
    int id = *(int *) arg;      // 1, 2, ...; solo lo usa la traza
    (void) id;
    int i;
    if (espera_ms > 0)
        usleep(espera_ms * 1000);   // deja que el consumidor corra primero
    for (i = 0; i < loops; i++) {
        TRAZA_PRINTF("P%d P0: sem_wait(&mutex)\n", id);
        sem_wait(&mutex);       // Line P0 (NEW LINE)
        TRAZA_PRINTF("P%d P1: tiene el mutex; sem_wait(&empty)\n", id);
        sem_wait(&empty);       // Line P1
        put(i);                 // Line P2
        sem_post(&full);        // Line P3
        sem_post(&mutex);       // Line P4 (NEW LINE)
    }
    return NULL;
}

void *consumer(void *arg) {
    consumidor_t *c = (consumidor_t *) arg;
    int tmp = 0;
    while (tmp != -1) {
        TRAZA_PRINTF("C%d C0: sem_wait(&mutex)\n", c->id);
        sem_wait(&mutex);       // Line C0 (NEW LINE)
        TRAZA_PRINTF("C%d C1: tiene el mutex; sem_wait(&full)\n", c->id);
        sem_wait(&full);        // Line C1
        tmp = get();            // Line C2
        sem_post(&empty);       // Line C3
        sem_post(&mutex);       // Line C4 (NEW LINE)
        printf("C%d <- %d\n", c->id, tmp);
        registra(c, tmp);
    }
    return NULL;
}

/* ---------------------------------- main ---------------------------------- */

int main(int argc, char *argv[]) {
    if (argc != 5 && argc != 6) {
        fprintf(stderr, "uso: %s <buffersize> <loops> <productores> <consumidores> [espera_ms]\n", argv[0]);
        exit(1);
    }
    max       = atoi(argv[1]);
    loops     = atoi(argv[2]);
    producers = atoi(argv[3]);
    consumers = atoi(argv[4]);
    if (argc == 6)
        espera_ms = atoi(argv[5]);
    assert(max >= 1 && loops >= 0);
    assert(producers >= 1 && producers <= PMAX);
    assert(consumers >= 1 && consumers <= CMAX);
    setvbuf(stdout, NULL, _IOLBF, 0);   // cada linea sale de inmediato (ver la guia)

    buffer = calloc(max, sizeof(int));
    assert(buffer != NULL);

    Sem_init(&empty, max);  // MAX espacios vacios
    Sem_init(&full, 0);     // 0 espacios llenos
    Sem_init(&mutex, 1);    // mutex = 1

    pthread_t pid[PMAX], cid[CMAX];
    int ids[PMAX];
    consumidor_t c[CMAX];
    int i;
    for (i = 0; i < producers; i++) {
        ids[i] = i + 1;
        Pthread_create(&pid[i], NULL, producer, &ids[i]);
    }
    for (i = 0; i < consumers; i++) {
        consumidor_init(&c[i], i + 1, loops);
        Pthread_create(&cid[i], NULL, consumer, &c[i]);
    }
    for (i = 0; i < producers; i++)
        Pthread_join(pid[i], NULL);

    // Marca de fin: un -1 por consumidor, con la misma sincronizacion del productor.
    for (i = 0; i < consumers; i++) {
        sem_wait(&mutex);
        sem_wait(&empty);
        put(-1);
        sem_post(&full);
        sem_post(&mutex);
    }
    for (i = 0; i < consumers; i++)
        Pthread_join(cid[i], NULL);

    verifica(c, consumers, producers, loops);
    free(buffer);
    return 0;
}
