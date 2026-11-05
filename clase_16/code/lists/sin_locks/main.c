#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "linked_list.h"


int END_VALUE = 50000;  // valor por defecto; se puede cambiar con el primer argumento
list_t L;

void* my_thread1(void* arg){
  // Insercion de numeros pares
  int num;
  for (num = 0; num < END_VALUE; num += 2){
    List_Insert(&L, num);
  }
  return NULL;
}

void* my_thread2(void* arg){
  // Insercion de numeros impares
  int num;
  for (num = 1; num < END_VALUE; num += 2){
    List_Insert(&L, num);
  }
  return NULL;
}


int main(int argc, char *argv[]) {

  int rc;
  int i;

  // Uso: ./main.out [END_VALUE]
  if (argc > 1) {
    END_VALUE = atoi(argv[1]);
    if (END_VALUE <= 0) {
      fprintf(stderr, "uso: %s [END_VALUE > 0]\n", argv[0]);
      return 1;
    }
  }

  // Inicializacion de la lista
  List_Init(&L);
  pthread_t thread1, thread2;

  // Creacion de los hilos
  rc = pthread_create(&thread1, NULL, my_thread1, NULL); // Hilo que inserta numeros pares
  assert(rc == 0);
  rc = pthread_create(&thread2, NULL, my_thread2, NULL); // Hilo que inserta numeros impares
  assert(rc == 0);

  // Espera de la terminacion de los hilos hijos
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  // El hilo principal verifica que se hayan agregado todos los elementos
  int cnt = 0;

  for(i = 0; i < END_VALUE; i++) {
    if(List_Lookup(&L, i) == 0) {
      // Numero encontrado
      cnt++;
    }
  }

  // Resumen de los resultados
  printf("- Cantidad esperada de valores insertados: %d\n", END_VALUE);
  printf("- Cantidad real de valores insertados: %d\n", cnt);

  // Primeros nodos de la lista (los ultimos insertados)
  node_t *curr = L.head;
  printf("- Primeros 10 nodos de la lista: ");
  for (i = 0; i < 10 && curr; i++) {
    printf("%d -> ", curr->key);
    curr = curr->next;
  }
  printf(curr ? "...\n" : "NULL\n");

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
