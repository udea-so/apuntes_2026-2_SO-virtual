#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include "my_cv.h" // Incluimos nuestra nueva estructura

/** --------------------------- Funciones del hilo ----------------------------- **/
void *child(void *arg) {
    // Recibimos el puntero a la variable de condicion
    my_cv *cv = (my_cv *)arg;

    sleep(2); // Simula trabajo
    printf("child\n");
    
    // Señalizamos al padre que hemos terminado
    my_cv_signal(cv); 
    
    return NULL;
}

/** ----------------------- Funcion Main -------------------------- **/
int main(int argc, char *argv[]) {
    
    // 1. Declaramos la estructura my_cv y el hilo
    my_cv cond;
    pthread_t c;

    // 2. Inicializamos la variable de condicion
    my_cv_init(&cond);

    printf("parent: begin\n");

    // 3. Creamos el hilo pasando la direccion de 'cond' como argumento
    // Esto evita el uso de variables globales
    Pthread_create(&c, NULL, child, &cond);
    
    // 4. El padre espera la señal del hijo
    my_cv_wait(&cond); 
    
    printf("parent: end\n");
    
    return 0;
}