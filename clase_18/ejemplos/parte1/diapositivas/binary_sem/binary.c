/*
 * binary.c - Semáforo binario usado como lock (diapositivas p. 13-17;
 *            OSTEP cap. 31, Fig. 31.3).
 *
 * Dos hilos incrementan 10 000 000 de veces cada uno un contador
 * compartido. El incremento (la sección crítica) va entre sem_wait() y
 * sem_post() sobre un semáforo inicializado en 1, así que el resultado
 * siempre es 20 000 000.
 *
 * Compilando con -DSIN_SEMAFORO (make genera binary-sin-semaforo.out) se
 * quitan sem_wait() y sem_post(): los dos hilos entran a la vez a la
 * sección crítica y se pierden incrementos (condición de carrera).
 *
 * Uso:         ./binary.out
 *              ./binary-sin-semaforo.out
 */
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
#ifndef SIN_SEMAFORO
    Sem_wait(&mutex);
#endif
    // Critical section
    counter++;
#ifndef SIN_SEMAFORO
    Sem_post(&mutex);
#endif
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
