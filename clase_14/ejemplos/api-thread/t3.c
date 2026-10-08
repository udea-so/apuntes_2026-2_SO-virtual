#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
1. To compile:

gcc -o t3 t3.c -lpthread -Wall

2. To run

./t3
*/

// Paso de un valor simple a un hilo (y de vuelta), sin struct ni malloc:
// el valor viaja directamente dentro del void *.
// Adaptado de thread_create_simple_args.c (OSTEP, ostep-code/threads-api).
//
// Version de las diapositivas (con int). En un sistema de 64 bits
// sizeof(int) = 4 y sizeof(void *) = 8: no la uses, es el ejercicio
// "encuentra el error" del README.
//
// void *mythread(void *arg) {
//     int m = (int) arg;
//     printf("%d\n", m);
//     return (void *) (arg + 1);
// }
//
// int main(int argc, char *argv[]) {
//     pthread_t p;
//     int rc, m;
//     pthread_create(&p, NULL, mythread, (void *) 100);
//     pthread_join(p, (void **) &m);
//     printf("returned %d\n", m);
//     return 0;
// }

void *mythread(void *arg) {
    long long int value = (long long int) arg; // mismo tamaño que void *
    printf("%lld\n", value);
    return (void *) (value + 1);
}

int main(int argc, char *argv[]) {
    pthread_t p;
    long long int rvalue; // pthread_join escribe aqui un void * (8 bytes)
    pthread_create(&p, NULL, mythread, (void *) 100);
    pthread_join(p, (void **) &rvalue);
    printf("returned %lld\n", rvalue);
    return 0;
}
