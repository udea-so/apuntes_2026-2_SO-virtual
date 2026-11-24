/*
 * dining_philosophers_no_deadlock.c - Filosofos comensales: el filosofo 4
 * toma los palillos en el orden contrario (derecho y luego izquierdo), lo que
 * rompe la espera circular. Solucion CORRECTA (sin deadlock).
 *
 * Diapositivas (parte 2) p. 45-46; OSTEP cap. 31, §31.6, Fig. 31.16.
 *
 * Codigo de ostep-code/threads-sema (adaptado: mensajes y salida por linea).
 *
 * Compilacion: gcc -Wall -I. -o dining_philosophers_no_deadlock.out dining_philosophers_no_deadlock.c -pthread
 * Uso:         ./dining_philosophers_no_deadlock.out <num_loops>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"
#include "common_threads.h"

#ifdef linux
#include <semaphore.h>
#elif __APPLE__
#include "zemaphore.h"
#endif

typedef struct {
    int num_loops;
    int thread_id;
} arg_t;

sem_t forks[5];

int left(int p)  {
    return p;
}

int right(int p) {
    return (p + 1) % 5;
}

void get_forks(int p) {
    if (p == 4) {
	Sem_wait(&forks[right(p)]);
	Sem_wait(&forks[left(p)]);
    } else {
	Sem_wait(&forks[left(p)]);
	Sem_wait(&forks[right(p)]);
    }
}

void put_forks(int p) {
    Sem_post(&forks[left(p)]);
    Sem_post(&forks[right(p)]);
}

void think() {
    return;
}

void eat() {
    return;
}

void *philosopher(void *arg) {
    arg_t *args = (arg_t *) arg;
    int p = args->thread_id;

    int i;
    for (i = 0; i < args->num_loops; i++) {
	think();
	get_forks(p);
	eat();
	put_forks(p);
    }
    return NULL;
}
                                                                             
int main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr, "uso: %s <num_loops>\n", argv[0]);
	exit(1);
    }
    setvbuf(stdout, NULL, _IOLBF, 0);   // cada linea sale de inmediato
    printf("dining: started\n");
    
    int i;
    for (i = 0; i < 5; i++) 
	Sem_init(&forks[i], 1);

    pthread_t p[5];
    arg_t a[5];
    for (i = 0; i < 5; i++) {
	a[i].num_loops = atoi(argv[1]);
	a[i].thread_id = i;
	Pthread_create(&p[i], NULL, philosopher, &a[i]);
    }

    for (i = 0; i < 5; i++) 
	Pthread_join(p[i], NULL); 

    printf("dining: finished\n");
    return 0;
}
