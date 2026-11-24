/*
 * pc_work.c - Productor/consumidor con buffer circular, empty, full y un
 * semaforo binario mutex que envuelve SOLO put() y get() (se reduce el ambito
 * del lock). Solucion CORRECTA con cualquier numero de productores y
 * consumidores.
 *
 * Diapositivas (parte 2) p. 22-25; OSTEP cap. 31, Fig. 31.12
 * (Adding Mutual Exclusion (Correctly)).
 *
 * Las etiquetas de linea son las de la p. 25 (P1-P5, C1-C5); entre parentesis,
 * las de OSTEP (P1, P1.5, P2, P2.5, P3).
 *
 * Compilacion: gcc -Wall -I. -o pc_work.out pc_work.c -pthread
 * Uso:         ./pc_work.out <buffersize> <loops> <productores> <consumidores>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"
#include "verifica.h"

int max;                  // MAX en las diapositivas: tamanio del buffer
int loops;
int producers;
int consumers;

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
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty);       // Line P1  (OSTEP: P1)
        sem_wait(&mutex);       // Line P2  (OSTEP: P1.5, lock)
        put(i);                 // Line P3  (OSTEP: P2)
        sem_post(&mutex);       // Line P4  (OSTEP: P2.5, unlock)
        sem_post(&full);        // Line P5  (OSTEP: P3)
    }
    return NULL;
}

void *consumer(void *arg) {
    consumidor_t *c = (consumidor_t *) arg;
    int tmp = 0;
    while (tmp != -1) {
        sem_wait(&full);        // Line C1  (OSTEP: C1)
        sem_wait(&mutex);       // Line C2  (OSTEP: C1.5, lock)
        tmp = get();            // Line C3  (OSTEP: C2)
        sem_post(&mutex);       // Line C4  (OSTEP: C2.5, unlock)
        sem_post(&empty);       // Line C5  (OSTEP: C3)
        printf("C%d <- %d\n", c->id, tmp);
        registra(c, tmp);
    }
    return NULL;
}

/* ---------------------------------- main ---------------------------------- */

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "uso: %s <buffersize> <loops> <productores> <consumidores>\n", argv[0]);
        exit(1);
    }
    max       = atoi(argv[1]);
    loops     = atoi(argv[2]);
    producers = atoi(argv[3]);
    consumers = atoi(argv[4]);
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
    consumidor_t c[CMAX];
    int i;
    for (i = 0; i < producers; i++)
        Pthread_create(&pid[i], NULL, producer, NULL);
    for (i = 0; i < consumers; i++) {
        consumidor_init(&c[i], i + 1, loops);
        Pthread_create(&cid[i], NULL, consumer, &c[i]);
    }
    for (i = 0; i < producers; i++)
        Pthread_join(pid[i], NULL);

    // Marca de fin: un -1 por consumidor, cuando ya no quedan productores.
    for (i = 0; i < consumers; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);
        put(-1);
        sem_post(&mutex);
        sem_post(&full);
    }
    for (i = 0; i < consumers; i++)
        Pthread_join(cid[i], NULL);

    verifica(c, consumers, producers, loops);
    free(buffer);
    return 0;
}
