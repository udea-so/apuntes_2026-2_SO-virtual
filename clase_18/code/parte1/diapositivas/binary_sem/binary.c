#include "common.h"
#include "common_threads.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** ----------- Variables globales ----------- **/
volatile int counter = 0;

/** ----- Variables de sincronizacion -------- **/

// Semaforo binario
sem_t mutex;

/** ------ Funcion del hijo ---------------- **/
void *child(void *arg) {
  int i;
  for (i = 0; i < 10000000; i++) {
    Sem_wait(&mutex);
    // Critical section
    counter++;
    Sem_post(&mutex);
  }
  return NULL;
}

/** ----------- Funcion Main --------------- **/
int main(int argc, char *argv[]) {

  // ---------  Inicializacion del semaforo  ---------
  Sem_init(&mutex, 1);
  // --------- Declaracion de los Hilos  -------------
  pthread_t c1, c2;
  // ---------- Inicializacion de los Hilos  ----------
  Pthread_create(&c1, NULL, child, NULL);
  Pthread_create(&c2, NULL, child, NULL);
  // ---------  Espera  ------------
  Pthread_join(c1, NULL);
  Pthread_join(c2, NULL);
  printf("result: %d (should be 20000000)\n", counter);
  return 0;
}
