/*
 * test_lock.c - "Código de test" del lock con semáforos (diapositiva p. 20).
 *
 * Crea <num> hilos. Cada uno adquiere el semáforo, incrementa el contador
 * compartido 1 000 000 de veces y lo libera, así que el resultado esperado
 * es <num> x 1 000 000. Es el código de la diapositiva, con las respuestas a
 * sus tres preguntas "What goes here?": sem_wait(&lock), sem_post(&lock) y
 * el valor inicial 1.
 *
 * El segundo argumento, opcional, cambia el valor inicial del semáforo para
 * responder la pregunta de la p. 13 ("¿cuál debe ser el valor de X?"):
 *   - 1 (por defecto): un solo hilo a la vez en la sección crítica.
 *   - 0: ningún hilo puede entrar; todos quedan bloqueados en sem_wait().
 *   - 2 o más: varios hilos entran a la vez y se pierden incrementos.
 *
 * Uso:         ./test_lock.out <num> [valor_inicial]
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"

#define PMAX 32

volatile int counter = 0;   // contador compartido

sem_t lock;

void *worker(void *arg) {
    int i;
    sem_wait(&lock);            // What goes here?
    for (i = 0; i < 1e6; i++)
        counter++;
    sem_post(&lock);            // What goes here?
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: %s <num> [valor_inicial]\n", argv[0]);
        exit(1);
    }
    int num = atoi(argv[1]);
    int valor_inicial = 1;      // agregado: por defecto, el valor de la diapositiva
    if (argc == 3)
        valor_inicial = atoi(argv[2]);
    assert(num >= 1 && num <= PMAX);

    pthread_t pid[PMAX];
    sem_init(&lock, 0, valor_inicial /* What goes here? */);
    for (int i = 0; i < num; i++)
        Pthread_create(&pid[i], 0, worker, 0);
    for (int i = 0; i < num; i++)
        Pthread_join(pid[i], NULL);
    printf("counter: %d\n", counter);
    return 0;
}
