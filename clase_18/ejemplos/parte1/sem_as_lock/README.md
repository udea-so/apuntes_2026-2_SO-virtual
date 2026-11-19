![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Lock implementado con un semáforo

Las diapositivas p. 18–20 construyen un **lock** con la interfaz de las clases 15 y 16 (`init`, `acquire`, `release`) usando un semáforo, y lo prueban con un contador compartido. Esta carpeta tiene la implementación del lock y dos programas de prueba.

Volver al [índice de la parte 1](../README.md).

## Relación con las diapositivas y el libro

| Programa | Qué muestra | Diapositivas | OSTEP cap. 31 (v1.10) |
|---|---|---|---|
| [`my_lock.h`](my_lock.h), [`my_lock.c`](my_lock.c) | `lock_t` con un semáforo: `init`, `acquire`, `release` | p. 18–19 | §31.2 |
| [`binary.c`](binary.c) → `binary.out` | Dos hilos que toman el lock en **cada** incremento | p. 19 | Fig. 31.3 |
| [`test_lock.c`](test_lock.c) → `test_lock.out` | "Código de test": `N` hilos que toman el semáforo una vez cada uno | p. 20 | — |

## Compilación

Desde esta carpeta:

```bash
make          # genera binary.out y test_lock.out
make clean    # borra los ejecutables
```

`binary.out` se arma con dos archivos fuente, el programa y la implementación del lock:

```bash
gcc -Wall -I. -pthread -o binary.out binary.c my_lock.c
gcc -Wall -I. -pthread -o test_lock.out test_lock.c
```

## El lock (p. 18–19)

La p. 18 plantea el ejercicio: con las operaciones del semáforo (`sem_init`, `sem_wait`, `sem_post`), completar la estructura `lock_t` y las funciones `init`, `acquire` y `release`. La solución de la p. 19:

```c
typedef struct __lock_t {
    sem_t sem;                  // la diapositiva lo llama value
} lock_t;

void init(lock_t *lock) {
    Sem_init(&lock->sem, 1);    // sem_init(&lock->sem, 0, 1): el lock empieza libre
}

void acquire(lock_t *lock) {
    Sem_wait(&lock->sem);       // lock
}

void release(lock_t *lock) {
    Sem_post(&lock->sem);       // unlock
}
```

Es decir, un lock es un semáforo binario: `acquire` es `sem_wait` y `release` es `sem_post`, sobre un semáforo que empieza en 1.

La p. 18 describe además el semáforo binario como uno con valor **booleano**: `bsem_wait()` espera a que el valor sea 1 y lo pone en 0, y `bsem_signal()` lo pone en 1 y despierta a un hilo. Es el comportamiento que se obtiene con un semáforo general inicializado en 1 cuando cada hilo hace un `sem_wait()` seguido de un `sem_post()`, como en esta carpeta.

## Prueba 1: `binary.c`, el lock en cada incremento

Es el programa de [`diapositivas/binary_sem/`](../diapositivas/binary_sem/README.md) escrito con la interfaz `lock_t`: dos hilos incrementan el contador 10 000 000 de veces cada uno y toman el lock en cada incremento.

```c
void *child(void *arg) {
    int i;
    for (i = 0; i < 10000000; i++) {
        acquire(&mutex);
        counter++;
        release(&mutex);
    }
    return NULL;
}
```

Salida de referencia:

```text
$ time ./binary.out
result: 20000000 (should be 20000000)

real	0m1.227s
user	0m0.781s
sys	0m1.563s
```

El resultado fue exacto en 20 de 20 ejecuciones.

## Prueba 2: `test_lock.c`, el código de test (p. 20)

Es el código de la diapositiva p. 20, con las respuestas a sus tres preguntas "*What goes here?*": `sem_wait(&lock)`, `sem_post(&lock)` y el valor inicial 1.

```c
sem_t lock;

void *worker(void *arg) {
    int i;
    sem_wait(&lock);            // What goes here?
    for (i = 0; i < 1e6; i++)
        counter++;
    sem_post(&lock);            // What goes here?
    return NULL;
}

int main(int argc, char *argv[]) {
    int num = atoi(argv[1]);
    pthread_t pid[PMAX];
    sem_init(&lock, 0, 1 /* What goes here? */);
    for (int i = 0; i < num; i++)
        Pthread_create(&pid[i], 0, worker, 0);
    for (int i = 0; i < num; i++)
        Pthread_join(pid[i], NULL);
    printf("counter: %d\n", counter);
    return 0;
}
```

La diapositiva no muestra la declaración de `counter` ni de `PMAX`; en el programa, `counter` es un `volatile int` global y `PMAX` vale 32. El número de hilos es obligatorio:

```bash
./test_lock.out <num> [valor_inicial]
```

Salida de referencia:

```text
$ time ./test_lock.out 4
counter: 4000000

real	0m0.027s
user	0m0.016s
sys	0m0.000s
```

| Ejecución | Resultado | `real` |
|---|---|---|
| `./test_lock.out 1` | 1 000 000 | 0.017 s |
| `./test_lock.out 2` | 2 000 000 | 0.021 s |
| `./test_lock.out 4` | 4 000 000 (exacto en 20 de 20 ejecuciones) | 0.027 s |
| `./test_lock.out 8` | 8 000 000 | 0.040 s |

### Grano grueso frente a grano fino

`test_lock.c` toma el semáforo **una vez por hilo**, alrededor de todo el ciclo: es un lock de **grano grueso**. Un hilo hace su millón de incrementos sin interrupciones de los demás, y los hilos pasan por la sección crítica uno detrás de otro. `binary.c`, en cambio, toma el lock en **cada** incremento (**grano fino**). Los dos programas son correctos, pero `test_lock.out 8` hizo 8 millones de incrementos en 0.04 s, mientras que `binary.out` necesitó 1.2 s para 20 millones: casi todo ese tiempo se fue en los 20 millones de pares `sem_wait()`/`sem_post()` y en dormir y despertar hilos. Es el compromiso entre granularidad y concurrencia de la clase 15: con grano grueso hay menos costo de sincronización, pero ningún paralelismo dentro de la sección crítica.

### ¿Cuál debe ser el valor inicial? (p. 13)

El segundo argumento, opcional, cambia el valor inicial del semáforo:

| Valor inicial | Qué pasa | Ejecución de referencia |
|---|---|---|
| **1** (correcto) | Un solo hilo a la vez en la sección crítica | `./test_lock.out 4` → 4 000 000 |
| **0** | Ningún hilo puede pasar `sem_wait()`: todos quedan bloqueados y el programa no termina | `timeout 2 ./test_lock.out 4 0` → código de salida 124 (lo terminó `timeout`) |
| **2** | Dos hilos entran a la vez a la sección crítica y se pierden incrementos | `./test_lock.out 4 2` → entre 2 037 004 y 2 525 800 en 20 ejecuciones (esperado: 4 000 000) |
| **4** | Los cuatro hilos entran a la vez | `./test_lock.out 4 4` → entre 1 093 797 y 1 519 003 en 20 ejecuciones |

Con un valor inicial `N`, el semáforo deja pasar hasta `N` hilos a la vez: es un **semáforo general** (*counting semaphore*, p. 12), útil para administrar un recurso con `N` unidades, pero no para la exclusión mutua.

```text
$ timeout 2 ./test_lock.out 4 0; echo $?
124

$ ./test_lock.out 2 2
counter: 1113429
```

## Para probar por cuenta propia

1. ¿Qué pasa si `release()` se llama antes de `counter++` en `binary.c`? ¿Y si un hilo llama a `release()` dos veces seguidas?
2. Cambiar `test_lock.c` para que el semáforo se tome dentro del ciclo, en cada incremento, y medir con `time` las ejecuciones con 1, 2, 4 y 8 hilos. Comparar con la tabla de arriba.
3. Un `sem_wait()` sobre un semáforo ocupado duerme al hilo, mientras que un spinlock (clase 15) gira hasta que el lock se libera. ¿En qué situación podría convenir el spinlock?

## Referencias

- Diapositivas de la clase 18, parte 1, p. 12–13 y 18–20.
- R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, v1.10, cap. 31: [*Semaphores*](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf), §31.2.
- `man sem_init`, `man sem_wait`, `man sem_post`.

> [!IMPORTANT]
> **Sobre esta guía.** El código y la guía se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP. Las salidas son de una ejecución de referencia del 19/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14; en otra máquina los números concretos cambian. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente.
