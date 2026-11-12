#include "counter.h"

/*
Contador SIN lock (diapositivas p. 5): ninguna operacion es thread-safe.
c->value++ y c->value-- se compilan como leer, modificar y escribir, y dos
hilos pueden intercalarse entre esos pasos (race condition).
*/

void init(counter_t *c) {
    c->value = 0;
}

void increment(counter_t *c) {
    c->value++;
}

void decrement(counter_t *c) {
    c->value--;
}

int get(counter_t *c) {
    return c->value;
}
