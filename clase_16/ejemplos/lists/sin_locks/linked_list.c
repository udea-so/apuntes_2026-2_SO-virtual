#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

void List_Init(list_t *L)
{
    L->head = NULL;
}

int List_Insert(list_t *L, int key)
{
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL)
    {
        perror("malloc");
        return -1; // fail
    }
    new->key = key;
    new->next = L->head;
    L->head = new;
    return 0; // success
}

int List_Lookup(list_t *L, int key)
{
    node_t *curr = L->head;
    while (curr)
    {
        if (curr->key == key)
        {
            return 0; // success
        }
        curr = curr->next;
    }
    return -1; // failure
}
