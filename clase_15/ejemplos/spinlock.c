#include <stdio.h>


/*
To compile:
gcc -o spinlock spinlock.c -Wall

To run:
./spinlock

*/

// ---------------------------------------------------------------
// Primitiva hardware: test-and-set (instruccion xchg de x86)
// Retorna el valor ANTERIOR de *ptr y escribe new_val
// de forma atomica.
// ---------------------------------------------------------------
int TestAndSet(int *ptr, int new_val) {
    __asm__ __volatile__(
        "xchgl %0, %1"
        : "+r" (new_val),
          "+m" (*ptr)
        :
        : "memory");
    return new_val; // retorna el valor ANTERIOR de *ptr
}

// ---------------------------------------------------------------
// Spinlock — mismo esqueleto que las slides (figura 28.3 OSTEP)
// flag = 0: libre (free) | flag = 1: tomado (held)
// ---------------------------------------------------------------
typedef struct __lock_t {
    int flag;
} lock_t;

void init(lock_t *lock) {
    // 0: lock is available, 1: lock is held
    lock->flag = 0;
}

void lock(lock_t *lock) {
    while (TestAndSet(&lock->flag, 1) == 1)
        ; // spin-wait (do nothing)
}

void unlock(lock_t *lock) {
    TestAndSet(&lock->flag, 0);
}

int main(int argc, char *argv[]) {
    lock_t mutex;
    init(&mutex);

    // Adquiere el lock: flag era 0 (libre) -> escribe 1, retorna 0 -> sale del while
    printf("before lock:  flag = %d\n", mutex.flag);
    lock(&mutex);
    printf("after  lock:  flag = %d (lock adquirido)\n\n", mutex.flag);

    // Intenta adquirir el lock de nuevo: flag es 1 (tomado) -> retorna 1 -> spinnea
    // En un programa real este hilo giraria hasta que otro hilo libere el lock.
    // Aqui se muestra una sola iteracion del spin para no bloquear el programa.
    printf("before spin attempt:  flag = %d\n", mutex.flag);
    int old = TestAndSet(&mutex.flag, 1);
    printf("after  spin attempt:  flag = %d (old: %d) -> lock ocupado, seguir girando\n\n",
           mutex.flag, old);

    // Libera el lock: escribe 0, retorna 1
    printf("before unlock: flag = %d\n", mutex.flag);
    unlock(&mutex);
    printf("after  unlock: flag = %d (lock liberado)\n\n", mutex.flag);

    // Adquiere el lock de nuevo: flag volvio a 0 (libre) -> lo toma
    printf("before lock:  flag = %d\n", mutex.flag);
    lock(&mutex);
    printf("after  lock:  flag = %d (lock adquirido de nuevo)\n", mutex.flag);

    return 0;
}