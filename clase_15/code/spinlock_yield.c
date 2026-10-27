#include <stdio.h>
#include <sched.h>


/*
To compile:
gcc -o spinlock_yield spinlock_yield.c -Wall

To run:
./spinlock_yield

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
// yield(): mueve el hilo de estado running -> ready,
// cediendo voluntariamente la CPU al scheduler.
// Equivalente al yield() de las slides (sched_yield en Linux).
// ---------------------------------------------------------------
void yield() {
    sched_yield();
}

// ---------------------------------------------------------------
// Lock con yield — slide 54 (OSTEP figura 28.8)
// Mejora sobre spinlock puro: en lugar de girar consumiendo CPU,
// el hilo cede el procesador mientras espera.
//
// Sin yield:  o(threads * time_slice)   <- desperdicia CPU
// Con yield:  o(threads * ctx_switch)   <- mas eficiente
//
// Limitacion: puede existir inanicion si el scheduler
// no garantiza equidad entre los hilos en espera.
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
        yield(); // give up the CPU
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

    // Intenta adquirir el lock de nuevo: flag es 1 (tomado) -> retorna 1 -> yield
    // En un programa real este hilo cederia la CPU y volveria a intentar.
    // Aqui se muestra una sola iteracion para no bloquear el programa.
    printf("before yield attempt:  flag = %d\n", mutex.flag);
    int old = TestAndSet(&mutex.flag, 1);
    if (old == 1) {
        printf("after  yield attempt:  flag = %d (old: %d) -> lock ocupado, yield()\n\n",
               mutex.flag, old);
        yield(); // cede la CPU: running -> ready
    }

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