/*
 * dining_philosophers_deadlock_print.c - La misma version con deadlock, pero
 * cada filosofo imprime lo que hace (columna propia por filosofo).
 *
 * Diapositivas (parte 2) p. 42-44; OSTEP cap. 31, §31.6, Fig. 31.15.
 *
 * Codigo de ostep-code/threads-sema (adaptado: mensajes y salida por linea).
 *
 * Compilacion: gcc -Wall -I. -o dining_philosophers_deadlock_print.out dining_philosophers_deadlock_print.c -pthread
 * Uso:         ./dining_philosophers_deadlock_print.out <num_loops>
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
sem_t print_lock;

void space(int s) {
    Sem_wait(&print_lock);
    int i;
    for (i = 0; i < s * 10; i++)
	printf(" ");
}

void space_end() {
    Sem_post(&print_lock);
}

int left(int p)  {
    return p;
}

int right(int p) {
    return (p + 1) % 5;
}

void get_forks(int p) {
    space(p); printf("%d: try %d\n", p, left(p)); space_end();
    Sem_wait(&forks[left(p)]);
    space(p); printf("%d: try %d\n", p, right(p)); space_end();
    Sem_wait(&forks[right(p)]);
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

    space(args->thread_id); printf("%d: start\n", args->thread_id); space_end();

    int i;
    for (i = 0; i < args->num_loops; i++) {
	space(args->thread_id); printf("%d: think\n", args->thread_id); space_end();
	think();
	get_forks(args->thread_id);
	space(args->thread_id); printf("%d: eat\n", args->thread_id); space_end();
	eat();
	put_forks(args->thread_id);
	space(args->thread_id); printf("%d: done\n", args->thread_id); space_end();
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
    Sem_init(&print_lock, 1);

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
