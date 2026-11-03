#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdio.h>

// basic node structure
typedef struct __node_t {
  int key;
  struct __node_t *next;
} node_t;


// basic list structure (one used per list)
typedef struct __list_t {
  node_t *head;
} list_t;


void List_Init(list_t *L); 
int List_Insert(list_t *L, int key);
int List_Lookup(list_t *L, int key);

#endif