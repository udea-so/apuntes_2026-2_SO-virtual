/*
 * zem_test.c - Prueba del Zemaforo (zemaphore.h) en los tres usos del
 * capitulo: ordenar hilos (join), exclusion mutua (lock) y espera de varios
 * hilos, mostrando que el valor no baja de 0.
 *
 * Diapositivas (parte 2) p. 47-48; OSTEP cap. 31, §31.8, Fig. 31.17.
 *
 * Compilacion: gcc -Wall -I. -o zem_test.out zem_test.c -pthread
 * Uso:         ./zem_test.out <hilos> <incrementos>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "common_threads.h"
#include "zemaphore.h"

#define HMAX 16

int incrementos;
volatile int counter = 0;

Zem_t s_join;     // 1. orden: empieza en 0
Zem_t m;          // 2. lock: empieza en 1
Zem_t s_espera;   // 3. espera de varios hilos: empieza en 0

void *child(void *arg) {
    printf("  child\n");
    Zem_post(&s_join);          // signal here: child is done
    return NULL;
}

void *worker(void *arg) {
    int i;
    for (i = 0; i < incrementos; i++) {
        Zem_wait(&m);           // lock
        counter++;              // critical section
        Zem_post(&m);           // unlock
    }
    return NULL;
}

void *waiter(void *arg) {
    Zem_wait(&s_espera);        // el valor es 0: el hilo espera
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "uso: %s <hilos> <incrementos>\n", argv[0]);
        exit(1);
    }
    int hilos = atoi(argv[1]);
    incrementos = atoi(argv[2]);
    assert(hilos >= 1 && hilos <= HMAX);
    setvbuf(stdout, NULL, _IOLBF, 0);
    pthread_t t[HMAX];
    int i;

    // 1. El padre espera al hijo (como Fig. 31.6, con un Zemaforo en 0).
    printf("1. join\n");
    Zem_init(&s_join, 0);
    printf("  parent: begin\n");
    Pthread_create(&t[0], NULL, child, NULL);
    Zem_wait(&s_join);          // wait here for child
    printf("  parent: end\n");
    Pthread_join(t[0], NULL);

    // 2. Exclusion mutua: <hilos> hilos incrementan un contador (Zemaforo en 1).
    printf("2. lock\n");
    Zem_init(&m, 1);
    for (i = 0; i < hilos; i++)
        Pthread_create(&t[i], NULL, worker, NULL);
    for (i = 0; i < hilos; i++)
        Pthread_join(t[i], NULL);
    printf("  counter: %d (esperado %d)\n", counter, hilos * incrementos);

    // 3. <hilos> hilos esperan en un Zemaforo en 0: el valor sigue en 0.
    printf("3. espera de %d hilos\n", hilos);
    Zem_init(&s_espera, 0);
    for (i = 0; i < hilos; i++)
        Pthread_create(&t[i], NULL, waiter, NULL);
    usleep(200000);             // 0.2 s: tiempo para que todos lleguen a Zem_wait
    printf("  valor con %d hilos esperando: %d\n", hilos, Zem_getvalue(&s_espera));
    for (i = 0; i < hilos; i++)
        Zem_post(&s_espera);
    for (i = 0; i < hilos; i++)
        Pthread_join(t[i], NULL);
    printf("  valor despues de %d posts: %d\n", hilos, Zem_getvalue(&s_espera));
    return 0;
}
