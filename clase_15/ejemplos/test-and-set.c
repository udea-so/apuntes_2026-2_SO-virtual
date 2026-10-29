#include <stdio.h>


/*
To compile:
gcc -o test-and-set test-and-set.c -Wall

To run:
./test-and-set

*/

int global = 0;

int test_and_set(int *ptr, int new_val) {
    // xchg intercambia atomicamente *ptr con new_val
    // y retorna el valor ANTERIOR de *ptr.
    // Nota: xchg tiene el prefijo lock implicito en x86,
    // no es necesario declararlo explicitamente.
    __asm__ __volatile__(
        "xchgl %0, %1"
        : "+r" (new_val),   // new_val: entrada con el nuevo valor, salida con el valor anterior
          "+m" (*ptr)       // *ptr:    entrada y salida (recibe new_val)
        :
        : "memory");
    return new_val;         // devuelve lo que habia en *ptr antes del swap
}

int main(int argc, char *argv[]) {
    // Toma el lock: global era 0 (libre) -> escribe 1, retorna 0
    printf("before acquiring lock: %d\n", global);
    int old = test_and_set(&global, 1);
    printf("after  acquiring lock: %d (old: %d)\n", global, old);

    // Intenta tomar el lock de nuevo: global ya es 1 (tomado) -> retorna 1
    printf("before acquiring lock (already held): %d\n", global);
    old = test_and_set(&global, 1);
    printf("after  acquiring lock (already held): %d (old: %d)\n", global, old);

    // Libera el lock: escribe 0, retorna 1
    printf("before releasing lock: %d\n", global);
    old = test_and_set(&global, 0);
    printf("after  releasing lock: %d (old: %d)\n", global, old);

    return 0;
}