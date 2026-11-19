/*
 * ejemplo1.c - El padre espera al hijo con espera activa (spin).
 *
 * Diapositivas p. 10 (Spin join); OSTEP cap. 30, Fig. 30.2.
 *
 * Compilacion: gcc -Wall -o ejemplo1.out ejemplo1.c -pthread
 * Uso:         ./ejemplo1.out [segundos]
 *
 *   segundos (opcional, por defecto 0): tiempo que el hijo duerme antes de
 *   imprimir "child". Permite medir con `time` la CPU que gasta el padre
 *   mientras gira en el while.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int done = 0;
int espera = 0;   // segundos que el hijo tarda en terminar

void *child(void *arg) {
  if (espera > 0)
    sleep(espera);
  printf("child\n");
  done = 1;
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc > 1)
    espera = atoi(argv[1]);
  printf("parent: begin\n");
  pthread_t c;
  pthread_create(&c, NULL, child, NULL); // create child
  while (done == 0)
    ; // spin
  printf("parent: end\n");
  return 0;
}
