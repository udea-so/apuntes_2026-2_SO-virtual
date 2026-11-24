/*
 * pc_single.c - Productor/consumidor con semaforos y buffer de UN elemento
 * (MAX = 1): dos semaforos, empty y full, sin mutex. Solucion CORRECTA.
 *
 * Diapositivas (parte 2) p. 10-12; OSTEP cap. 31, Figs. 31.9 y 31.10 con MAX = 1.
 *
 * Compilacion: gcc -Wall -I. -o pc_single.out pc_single.c -pthread
 * Uso:         ./pc_single.out <loops> <productores> <consumidores>
 *
 *   Cada productor pone los valores 0, 1, ..., loops-1 y cada consumidor
 *   imprime lo que saca ("C1 <- 5"). Cuando terminan los productores, main
 *   pone un -1 por consumidor (marca de fin) y al final verifica que cada
 *   valor se haya consumido exactamente <productores> veces.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"
#include "verifica.h"

int loops;
int producers;
int consumers;

/* ------------------------- Buffer de un elemento -------------------------- */

int buffer;               // MAX = 1

void put(int value) {
    buffer = value;       // Line F1
}

int get(void) {
    int tmp = buffer;     // Line G1
    return tmp;
}

/* ------------------------------ Semaforos --------------------------------- */

sem_t empty;              // espacios vacios: empieza en MAX = 1
sem_t full;               // espacios llenos: empieza en 0

/* ------------------------- Productor y consumidor ------------------------- */

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty);       // Line P1
        put(i);                 // Line P2
        sem_post(&full);        // Line P3
    }
    return NULL;
}

void *consumer(void *arg) {
    consumidor_t *c = (consumidor_t *) arg;
    int tmp = 0;
    while (tmp != -1) {
        sem_wait(&full);        // Line C1
        tmp = get();            // Line C2
        sem_post(&empty);       // Line C3
        printf("C%d <- %d\n", c->id, tmp);
        registra(c, tmp);
    }
    return NULL;
}

/* ---------------------------------- main ---------------------------------- */

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "uso: %s <loops> <productores> <consumidores>\n", argv[0]);
        exit(1);
    }
    loops     = atoi(argv[1]);
    producers = atoi(argv[2]);
    consumers = atoi(argv[3]);
    assert(loops >= 0);
    assert(producers >= 1 && producers <= PMAX);
    assert(consumers >= 1 && consumers <= CMAX);
    setvbuf(stdout, NULL, _IOLBF, 0);   // cada linea sale de inmediato (ver la guia)

    Sem_init(&empty, 1);    // sem_init(&empty, 0, 1): MAX = 1 espacio vacio
    Sem_init(&full, 0);     // sem_init(&full, 0, 0):  0 espacios llenos

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
        put(-1);
        sem_post(&full);
    }
    for (i = 0; i < consumers; i++)
        Pthread_join(cid[i], NULL);

    verifica(c, consumers, producers, loops);
    return 0;
}
