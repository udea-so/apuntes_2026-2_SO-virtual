#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include <semaphore.h>


/*
void Sem_init(sem, value) {
  assert(sem_init(sem, 0, value) == 0);
}
void Sem_wait(sem) {                                  
  assert(sem_wait(sem) == 0);
}
void define Sem_post(sem) {                              
  assert(sem_post(sem) == 0);
}
*/

/** ----------------------- Variables de sincronizacion ------------------------- **/
// Semaforo que funcionara como variable de condicion
sem_t s;
/** ------------------------------------------------------------------------------ **/

/** --------------------------- Funciones del buffer ----------------------------- **/
void *child(void *arg) {
  sleep(2);
  printf("child\n");
  Sem_post(&s); // signal here: child is done
  return NULL;
}

/** ----------------------- Funcion Main -------------------------- **/
int main(int argc, char *argv[]) {  
    

    // -----------------  Inicializacion del semaforo  -----------------
    Sem_init(&s, 0); 
    printf("parent: begin\n");
    // ----------------- Declaracion de los Hilos  -----------------
    pthread_t c;
    // ----------------- Inicializacion de los Hilos  -----------------
    Pthread_create(&c, NULL, child, NULL);
    
    // -----------------  Espera  ----------------- 
    Sem_wait(&s); // wait here for child
    printf("parent: end\n");
    return 0;
}


