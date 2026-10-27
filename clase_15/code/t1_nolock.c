#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

/*
Ejemplo SIN lock: demuestra la race condition (condicion de carrera)
al incrementar un contador compartido desde dos hilos concurrentes.

Comparar con t1_lock.c para ver el efecto de agregar un mutex.

1. To compile:
   gcc -O0 -o t1_nolock t1_nolock.c -lpthread -Wall

2. To run:
   ./t1_nolock <MAX_VALUE>

   Ejemplo:
   ./t1_nolock 1000

3. Salida esperada (con race condition):

   main: begin [counter = 0]
   A: done
   B: done
   main: done
   [counter: 1001]      <- valor INCORRECTO (deberia ser 2000)
   [should:  2000]

   Nota: el valor del counter sera casi siempre menor que (max * 2)
   y puede variar entre ejecuciones.

4. Por que ocurre la race condition?

   La linea  counter = counter + 1  genera 3 instrucciones en ensamblador:

      mov 0x<addr>, %eax     <- (1) lee counter desde memoria al registro
      add $0x1,     %eax     <- (2) incrementa el registro
      mov %eax,     0x<addr> <- (3) escribe el resultado en memoria

   Si el scheduler interrumpe un hilo entre (1) y (3) y el otro hilo
   ejecuta sus 3 pasos completos, la escritura del segundo hilo se
   pierde cuando el primer hilo ejecuta (3) con su valor desactualizado.

   sched_yield() fuerza ese cambio de contexto para que la race condition
   sea siempre observable. En produccion ocurre igual pero de forma
   no determinista.
*/

int max;
int counter = 0; // shared global variable - SIN proteccion

void *mythread(void *arg) {
    char *letter = arg;
    int i; // stack (private per thread)
    for (i = 0; i < max; i++) {
        int tmp = counter;  // (1) lee counter en variable local (registro)
        sched_yield();      // fuerza cambio de contexto: el otro hilo corre aqui
        tmp = tmp + 1;      // (2) incrementa la copia local
        counter = tmp;      // (3) escribe -- puede pisar el valor del otro hilo
    }
    printf("%s: done\n", letter);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: t1_nolock <loopcount>\n");
        exit(1);
    }
    max = atoi(argv[1]);

    pthread_t p1, p2;
    printf("main: begin [counter = %d]\n", counter);
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: done\n");
    printf("[counter: %d]\n[should:  %d]\n", counter, max * 2);
    return 0;
}