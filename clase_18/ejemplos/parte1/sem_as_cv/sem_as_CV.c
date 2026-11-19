/*
 * sem_as_CV.c - El padre espera al hijo usando la interfaz my_cv
 *               (my_cv.c), que encapsula un semáforo inicializado en 0.
 *
 * Es el mismo programa de diapositivas/ordering_sem/sem_as_CV.c
 * (diapositivas p. 21-26), reorganizado: el semáforo queda oculto detrás de
 * my_cv_init(), my_cv_wait() y my_cv_signal(), y se le pasa al hijo como
 * argumento en lugar de usar una variable global. No aparece en las
 * diapositivas.
 *
 * Los dos argumentos opcionales, en segundos, son los mismos de
 * ordering_sem: espera_hijo > 0 hace que el padre llame a my_cv_wait()
 * primero; espera_padre > 0 hace que el hijo llame a my_cv_signal() primero.
 *
 * Uso:         ./sem_as_CV.out [espera_hijo] [espera_padre]
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include "my_cv.h"

int espera_hijo = 0;   // segundos que se demora el hijo
int espera_padre = 0;  // segundos que se demora el padre

/** --------------------------- Funcion del hijo ----------------------------- **/
void *child(void *arg) {
    // Recibimos el puntero a la "variable de condicion"
    my_cv *cv = (my_cv *)arg;

    if (espera_hijo > 0)
        sleep(espera_hijo); // Simula trabajo
    printf("child\n");

    // Señalizamos al padre que hemos terminado
    my_cv_signal(cv);

    return NULL;
}

/** ----------------------- Funcion Main -------------------------- **/
int main(int argc, char *argv[]) {
    if (argc > 1)
        espera_hijo = atoi(argv[1]);
    if (argc > 2)
        espera_padre = atoi(argv[2]);

    // 1. Declaramos la estructura my_cv y el hilo
    my_cv cond;
    pthread_t c;

    // 2. Inicializamos la "variable de condicion" (semaforo en 0)
    my_cv_init(&cond);

    printf("parent: begin\n");

    // 3. Creamos el hilo pasando la direccion de 'cond' como argumento
    Pthread_create(&c, NULL, child, &cond);

    if (espera_padre > 0)
        sleep(espera_padre);

    // 4. El padre espera la señal del hijo
    my_cv_wait(&cond);

    printf("parent: end\n");

    return 0;
}
