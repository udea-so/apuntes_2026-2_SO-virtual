![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Zemáforos: un semáforo con un lock y una variable de condición

Las diapositivas de la parte 2 (p. 47–48) y OSTEP (§31.8) cierran el tema construyendo un semáforo propio con las primitivas de las clases 15 a 17: **un lock y una variable de condición**. OSTEP lo llama *Zemaphore* ("Zemáforo") para distinguirlo del semáforo de POSIX. Esta carpeta no estaba en el material del semestre pasado: se agregó para poder ejecutar el código de esas diapositivas.

Volver al [índice de la parte 3](../README.md).

## Relación con las diapositivas y el libro

| Programa | Qué muestra | Diapositivas | OSTEP cap. 31 (v1.10) |
|---|---|---|---|
| [`zemaphore.h`](zemaphore.h) | `Zem_t`, `Zem_init`, `Zem_wait`, `Zem_post` (y `Zem_getvalue`, que no está en OSTEP, para mostrar el valor) | p. 47 | Fig. 31.17 |
| [`zem_test.c`](zem_test.c) → `zem_test.out` | El Zemáforo usado para ordenar hilos (*join*), como lock y con varios hilos esperando | p. 48 | §31.8 |

## Compilación y uso

```bash
make          # genera zem_test.out
make clean

./zem_test.out <hilos> <incrementos>
```

## La implementación (p. 47, OSTEP Fig. 31.17)

```c
typedef struct __Zem_t {
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Zem_t;

// only one thread can call this
void Zem_init(Zem_t *s, int value) {
    s->value = value;
    Cond_init(&s->cond);
    Mutex_init(&s->lock);
}

void Zem_wait(Zem_t *s) {
    Mutex_lock(&s->lock);
    while (s->value <= 0)
        Cond_wait(&s->cond, &s->lock);
    s->value--;
    Mutex_unlock(&s->lock);
}

void Zem_post(Zem_t *s) {
    Mutex_lock(&s->lock);
    s->value++;
    Cond_signal(&s->cond);
    Mutex_unlock(&s->lock);
}
```

Es la receta de la clase 17 aplicada a un caso concreto: el **estado** es `value`, el **lock** lo protege y la **variable de condición** permite dormir mientras `value <= 0`, verificando en un `while`. `Zem_wait` espera a que haya una unidad disponible y la toma; `Zem_post` devuelve una unidad y despierta a un hilo.

La p. 47 lo compara con la definición del semáforo de la [parte 1](../../parte1/README.md) (p. 5):

| | Semáforo de Dijkstra (p. 5, OSTEP Fig. 31.2) | Zemáforo (p. 47, OSTEP Fig. 31.17) |
|---|---|---|
| `wait` | Decrementa; si el valor queda negativo, espera | Espera mientras el valor sea `<= 0`; luego decrementa |
| Valor con `N` hilos esperando | `−N` | `0` |
| Implementación | Necesita su propia cola de hilos y atomicidad (p. 5: *Must be executed atomically*) | Usa un lock y una variable de condición ya existentes |

La p. 48 resume la diferencia: en el Zemáforo **el valor no representa el número de hilos bloqueados** cuando es negativo, porque nunca baja de 0. Es más fácil de implementar y es el comportamiento de los semáforos de Linux.

## Resultados

```text
$ time ./zem_test.out 4 1000000
1. join
  parent: begin
  child
  parent: end
2. lock
  counter: 4000000 (esperado 4000000)
3. espera de 4 hilos
  valor con 4 hilos esperando: 0
  valor despues de 4 posts: 0

real	0m3.821s
user	0m0.609s
sys	0m6.984s
```

1. **Para ordenar hilos** (Zemáforo en 0): el padre espera al hijo y la salida tiene siempre el orden de la [parte 1](../../parte1/diapositivas/ordering_sem/README.md).
2. **Como lock** (Zemáforo en 1): cuatro hilos incrementan un contador un millón de veces cada uno y el resultado es exacto. Lo fue en 20 de 20 ejecuciones con 100 000 incrementos por hilo.
3. **Varios hilos esperando** (Zemáforo en 0): con cuatro hilos dormidos en `Zem_wait`, el valor es **0**. En el modelo de Dijkstra sería −4. Después de cuatro `Zem_post` cada hilo toma su unidad y el valor vuelve a 0.

El tiempo de la parte 2 (casi todo en `sys`) muestra el costo de un lock que duerme a los hilos: cada `Zem_wait` con el Zemáforo ocupado pasa por el kernel. Es el mismo efecto que se midió en la parte 1 con el semáforo de POSIX.

> [!NOTE]
> **Complemento (OSTEP §31.8): el camino inverso es difícil.** Construir un semáforo con un lock y una variable de condición es sencillo, como se ve arriba. Lo contrario, construir variables de condición con semáforos, es mucho más difícil: OSTEP cuenta que programadores con experiencia lo intentaron en Windows y aparecieron muchos errores. Por eso advierte (*Tip: Be Careful With Generalization*) que ver el semáforo como una generalización de los locks y las variables de condición no es tan general como parece. La guía de [`sem_as_cv`](../../parte1/sem_as_cv/README.md) de la parte 1 muestra una de las razones.

## Para probar por cuenta propia

1. Cambiar el `while` de `Zem_wait` por un `if`. ¿Qué podría pasar con varios hilos esperando? (Repasar la clase 17.)
2. Reemplazar `Zem_t` por `sem_t` en `zem_test.c` (con `sem_init`, `sem_wait`, `sem_post` y `sem_getvalue`) y comparar el valor que se muestra en la parte 3 y el tiempo de la parte 2.

## Referencias

- Diapositivas de la clase 18, parte 2, p. 47–48.
- R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, v1.10, cap. 31: [*Semaphores*](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf), §31.8 y Fig. 31.17; código en [`ostep-code/threads-sema/zemaphore.h`](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-sema).

> [!IMPORTANT]
> **Sobre esta guía.** El código y la guía se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP. Las salidas son de una ejecución de referencia del 23/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14; en otra máquina los tiempos cambian. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente.
