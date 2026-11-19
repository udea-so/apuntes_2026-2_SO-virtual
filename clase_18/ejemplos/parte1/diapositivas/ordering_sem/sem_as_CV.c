/*
 * sem_as_CV.c - Semáforo usado para imponer un orden: el padre espera al
 *               hijo (diapositivas p. 21-26; OSTEP cap. 31, Fig. 31.6).
 *
 * El semáforo se inicializa en 0. El hijo llama a sem_post() al terminar y
 * el padre llama a sem_wait(): si el hijo todavía no ha terminado, el padre
 * se bloquea; si ya terminó, el padre continúa sin bloquearse, porque el
 * semáforo "recuerda" el post (su valor quedó en 1).
 *
 * Sin argumentos es el código de las diapositivas. Dos argumentos
 * opcionales, en segundos, fuerzan cada uno de los dos órdenes que
 * analizan las diapositivas:
 *   - espera_hijo  > 0: el hijo se demora antes de imprimir, así que el
 *                       padre llama a sem_wait() primero y se bloquea.
 *   - espera_padre > 0: el padre se demora antes de sem_wait(), así que el
 *                       hijo llama a sem_post() primero.
 *
 * Compilando con -DTRAZA (make traza genera sem_as_CV-traza.out) se
 * imprime el valor del semáforo, obtenido con sem_getvalue(), en cada paso.
 *
 * Uso:         ./sem_as_CV.out [espera_hijo] [espera_padre]
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "common_threads.h"
#include <semaphore.h>

/** ----------------------- Variables de sincronizacion ------------------------- **/
// Semaforo usado para ordenar la ejecucion (inicializado en 0)
sem_t s;
/** ------------------------------------------------------------------------------ **/

int espera_hijo = 0;   // agregado: segundos que se demora el hijo
int espera_padre = 0;  // agregado: segundos que se demora el padre

#ifdef TRAZA
void traza(const char *quien, const char *paso) {
  int valor;
  sem_getvalue(&s, &valor);
  printf("  [%s] %-28s valor del semaforo = %d\n", quien, paso, valor);
}
#else
#define traza(quien, paso)
#endif

/** --------------------------- Funcion del hijo ----------------------------- **/
void *child(void *arg) {
  if (espera_hijo > 0)   // agregado: solo si se pide por argumento
    sleep(espera_hijo);
  printf("child\n");
  traza("hijo ", "antes de sem_post()");
  Sem_post(&s); // signal here: child is done
  traza("hijo ", "despues de sem_post()");
  return NULL;
}

/** ----------------------- Funcion Main -------------------------- **/
int main(int argc, char *argv[]) {
  if (argc > 1)
    espera_hijo = atoi(argv[1]);
  if (argc > 2)
    espera_padre = atoi(argv[2]);

  // -----------------  Inicializacion del semaforo  -----------------
  Sem_init(&s, 0); // X = 0
  printf("parent: begin\n");
  traza("padre", "despues de sem_init()");
  // ----------------- Declaracion de los Hilos  -----------------
  pthread_t c;
  // ----------------- Inicializacion de los Hilos  -----------------
  Pthread_create(&c, NULL, child, NULL);

  if (espera_padre > 0)  // agregado: solo si se pide por argumento
    sleep(espera_padre);

  // -----------------  Espera  -----------------
  traza("padre", "antes de sem_wait()");
  Sem_wait(&s); // wait here for child
  traza("padre", "despues de sem_wait()");
  printf("parent: end\n");
  return 0;
}
