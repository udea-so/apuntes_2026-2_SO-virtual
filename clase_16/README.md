# Clase 16 — Estructuras de datos basadas en locks

La clase 15 mostró cómo se construye un lock. Esta clase responde a la pregunta siguiente: **cómo agregar locks a una estructura de datos** para que sea *thread-safe* (que funcione correctamente cuando varios hilos la usan a la vez) sin que el desempeño caiga (diapositivas p. 3). El material sigue el capítulo 29 de OSTEP (*Lock-based Concurrent Data Structures*) con cuatro estructuras: el contador, la lista enlazada, la cola y la tabla hash.

- Diapositivas anotadas: [`SO_apuntes_clase_16_annotated.pdf`](SO_apuntes_clase_16_annotated.pdf)
- Ejemplos: [`ejemplos/`](ejemplos/README.md)

## Sesiones

| Fecha | Diapositivas | Temas |
|---|---|---|
| 03/11/2026 | p. 1–25 | Repaso de locks; contador sin lock, contador preciso y contador aproximado (*sloppy counter*) |
| 05/11/2026 | p. 26–45 | Lista enlazada sin y con lock, su versión mejorada y *hand-over-hand locking*; cola sin concurrencia. La cola concurrente (de Michael y Scott) y la tabla hash concurrente se presentaron y su desarrollo quedó para los videos del Prof. Danny Múnera |

## Ruta de lectura

Cada guía se puede leer por separado, pero este es el orden sugerido: cada paso usa lo visto en el anterior.

| Paso | Material | Diapositivas | OSTEP | Qué se observa |
|---|---|---|---|---|
| 0 | [Entorno de ejecución](../clase_14/ejemplos/entorno_ejecucion.md) (clase 14) | — | — | En qué máquina se obtuvieron las salidas de referencia y por qué los resultados en otra máquina pueden ser distintos |
| 1 | [Contador sin lock](ejemplos/counters/counter_without_locks/README.md) | p. 5–7 | §29.1, Fig. 29.1 | Con varios hilos se pierde la mayoría de los incrementos (race condition) |
| 2 | [Contador preciso](ejemplos/counters/precise_counter/README.md) | p. 4, 9–13 | §29.1, Figs. 29.2 y 29.5 | El patrón lock → región crítica → unlock hace el valor exacto, pero el contador escala pobremente |
| 3 | [Contador aproximado](ejemplos/counters/sloppy_counter/README.md) | p. 14–25 | §29.1, Figs. 29.3–29.6 | Contadores locales y un umbral S: el valor queda por debajo del real de forma predecible, y S regula exactitud vs. desempeño |
| 4 | [Preciso vs. aproximado sin *false sharing*](ejemplos/counters/comparison/README.md) | p. 12, 24 y 25 | §29.1, Figs. 29.5 y 29.6 | Por qué la versión de las diapositivas casi no escala en la máquina de referencia y cómo se reproducen las gráficas de OSTEP (complemento) |
| 5 | [Lista enlazada sin lock](ejemplos/lists/sin_locks/README.md) | p. 26–28 | §29.2 | Dos hilos que insertan a la vez pierden nodos |
| 6 | [Lista enlazada con lock](ejemplos/lists/con_locks/README.md) | p. 29–35 | §29.2, Figs. 29.7 y 29.8 | Un lock para toda la lista evita la pérdida; la versión mejorada lo toma solo alrededor de la región crítica |

El índice de los ejemplos de contadores está en [`ejemplos/counters/`](ejemplos/counters/README.md) y el de las listas en [`ejemplos/lists/`](ejemplos/lists/README.md).

### Temas sin ejemplo de código

- ***Hand-over-hand locking*** (p. 36, OSTEP §29.2, *Scaling Linked Lists*): un lock por nodo; al recorrer la lista se adquiere el lock del siguiente nodo antes de liberar el del actual. Permite más concurrencia, pero en la práctica el costo de tomar y soltar un lock por cada nodo lo hace más lento que la lista con un solo lock.
- **Cola concurrente** (p. 37–42, OSTEP §29.3, Fig. 29.9): en clase se vio la cola sin concurrencia (p. 37–39). La cola de Michael y Scott usa dos locks, uno para la cabeza y otro para la cola, y un nodo ficticio (*dummy*) creado en la inicialización, de modo que encolar y desencolar pueden ocurrir al mismo tiempo. Quedó para los videos del Prof. Danny Múnera.
- **Tabla hash concurrente** (p. 43–45, OSTEP §29.4, Figs. 29.10 y 29.11): un arreglo de listas concurrentes, con un lock por cubo (*bucket*); como operaciones sobre cubos distintos no compiten, escala mucho mejor que una sola lista. Quedó para los videos del Prof. Danny Múnera.

> [!NOTE]
> En el código de la cola de las diapositivas (p. 38 y 42), el nodo se define con el campo `key`, pero las funciones usan `tmp->value` y `new_head->value`. En OSTEP (Fig. 29.9) el campo se llama `value`, que es lo que espera el código.

## Ideas principales

- **Thread-safe con el patrón básico.** Agregar un lock a la estructura, adquirirlo antes de manipularla y liberarlo al terminar la hace correcta (diapositivas p. 4).
- **Correcto no significa escalable.** Un único lock serializa todas las operaciones: con más hilos, el tiempo crece (contador preciso, lista con un lock).
- **Más locks, menos competencia.** Repartir la estructura en partes con su propio lock reduce la competencia: contadores locales en el contador aproximado, un lock por cubo en la tabla hash, dos locks en la cola.
- **Más concurrencia no siempre es más rápido.** Cada lock tiene un costo; un lock por nodo (*hand-over-hand*) termina siendo más lento que uno solo (OSTEP, recuadro *More Concurrency Isn't Necessarily Faster*).
- **Cuidado con el flujo de control.** Cada salida de una función que tomó un lock debe liberarlo; por eso la versión mejorada de la lista toma el lock solo alrededor de la región crítica y tiene un único punto de salida (diapositivas p. 33–35).

## Referencias

1. Arpaci-Dusseau, R. y Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*, v1.10. Cap. 29, *Lock-based Concurrent Data Structures* [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf).
2. Cap. 28, *Locks*, de OSTEP (la clase anterior) [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf).
3. Documentación del kernel de Linux: *False Sharing* (para el paso 4) [[link]](https://docs.kernel.org/kernel-hacking/false-sharing.html).
