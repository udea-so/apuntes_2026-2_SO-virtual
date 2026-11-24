/*
 * pc_attemp1.c - Productor/consumidor con semaforos y buffer CIRCULAR de
 * <buffersize> elementos: dos semaforos, empty y full, sin mutex.
 * Solucion ROTA con mas de un productor o mas de un consumidor: put() y get()
 * son secciones criticas sin proteger (race condition).
 *
 * Diapositivas (parte 2) p. 13-16; OSTEP cap. 31, Figs. 31.9 y 31.10.
 *
 * Compilacion: gcc -Wall -I. -o pc_attemp1.out pc_attemp1.c -pthread
 * Uso:         ./pc_attemp1.out <buffersize> <loops> <productores> <consumidores> [pausa_us]
 *
 *   pausa_us (opcional, por defecto 0): microsegundos que put() espera entre
 *   las lineas F1 y F2. Simula que el productor es interrumpido justo despues
 *   de escribir en buffer[fill] y antes de avanzar fill (traza de la p. 16).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"
#include "verifica.h"

int max;                  // MAX en las diapositivas: tamanio del buffer
int loops;
int producers;
int consumers;
int pausa_us = 0;

/* ---------------------- Buffer circular (p. 10) --------------------------- */

int *buffer;              // int buffer[MAX];
int fill = 0;
int use  = 0;

void put(int value) {
    buffer[fill] = value;          // Line F1
    if (pausa_us > 0)
        usleep(pausa_us);          // el productor "es interrumpido" entre F1 y F2
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
    if (argc != 5 && argc != 6) {
        fprintf(stderr, "uso: %s <buffersize> <loops> <productores> <consumidores> [pausa_us]\n", argv[0]);
        exit(1);
    }
    max       = atoi(argv[1]);
    loops     = atoi(argv[2]);
    producers = atoi(argv[3]);
    consumers = atoi(argv[4]);
    if (argc == 6)
        pausa_us = atoi(argv[5]);
    assert(max >= 1 && loops >= 0);
    assert(producers >= 1 && producers <= PMAX);
    assert(consumers >= 1 && consumers <= CMAX);
    setvbuf(stdout, NULL, _IOLBF, 0);   // cada linea sale de inmediato (ver la guia)

    buffer = calloc(max, sizeof(int));
    assert(buffer != NULL);

    Sem_init(&empty, max);  // sem_init(&empty, 0, MAX): MAX espacios vacios
    Sem_init(&full, 0);     // sem_init(&full, 0, 0):    0 espacios llenos

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
    free(buffer);
    return 0;
}
