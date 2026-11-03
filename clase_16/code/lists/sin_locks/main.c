#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "linked_list.h"


int END_VALUE = 10000;
list_t L;

void* my_thread1(void* arg){
  // Insersion de numeros pares
  int num = 0;
  for (num = 0; num < END_VALUE; num += 2){
    List_Insert(&L, num);
  }
  return NULL;
}

void* my_thread2(void* arg){
  // Insersion de numeros impares
  int num;
  for (num = 1; num < END_VALUE; num += 2){
    List_Insert(&L, num);
  }
  return NULL;
}


int main() {

  int rc;
  int i;

  // Inicializacion de la lista
  List_Init(&L);
  pthread_t thread1, thread2;

  // Creacion de los hilos
  rc = pthread_create(&thread1, NULL, my_thread1, NULL); // Hilo que inserta numeros pares
  assert(rc == 0);
  rc = pthread_create(&thread2, NULL, my_thread2, NULL); // Hilo que inserta numero impares
  assert(rc == 0);
  
  // Espera de la terminacion de los hilos hijos
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  // El hilo principal verificara que se hayan agregado todos lo elementos
  int cnt = 0;

  for(i = 0; i < END_VALUE;i++) {
    if(List_Lookup(&L, i) == 0) {
      // Numero encontrado
      cnt++;
    }
  }

  // Resumen de los resultados
  printf("- Cantidad esperada de valores insertados: %d\n",END_VALUE);
  printf("- Cantidad real de valores insertados: %d\n",cnt);


  /*
  // Test del funcionamiento basico
  int found;
  list_t L;
  List_Init(&L);
  List_Insert(&L,3);
  List_Insert(&L,8);
  found = List_Lookup(&L, 3);
  found = List_Lookup(&L, 8);
  printf("3 in L? %d\n",found);
  found = List_Lookup(&L, 5);
  printf("3 in L? %d\n",found);
  */ 
  return 0;
}


