![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Contadores — Parte 2: contador preciso (con un único lock)

Segundo eslabón: el contador de la [Parte 1](../counter_without_locks/README.md) protegido con un `pthread_mutex_t`. El resultado pasa a ser siempre correcto, a cambio de un costo en desempeño que crece con el número de hilos. Corresponde a las diapositivas p. 9–13 y a OSTEP §29.1 (Fig. 29.2, *A Counter With Locks*, y Fig. 29.5).

Las salidas son de una ejecución de referencia del 11/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14 (WSL1, `gcc` 9.4.0, AMD Ryzen 5 3500U con 4 núcleos físicos y 8 lógicos). Los tiempos cambian de una ejecución a otra y de una máquina a otra.

---

## Archivos

| Archivo | Contenido |
|---|---|
| [`counter_lock.h`](counter_lock.h) / [`counter_lock.c`](counter_lock.c) | El contador de las diapositivas (p. 9): `counter_t` con `value` y `lock`, y cada operación rodeada por `pthread_mutex_lock`/`pthread_mutex_unlock`. |
| [`main_precise_counter.c`](main_precise_counter.c) | Programa de la diapositiva p. 10: 16 hilos incrementan un millón de veces el mismo contador. Genera `precise_counter`. |
| [`benchmark_precise_counter.c`](benchmark_precise_counter.c) | Repite la prueba con 1, 2, 4, 8 y 16 hilos y guarda los tiempos en `results_lock.csv`. Genera `benchmark_precise_counter`. |
| [`results_lock.csv`](results_lock.csv) | Resultados de la ejecución de referencia. |
| [`pruebas_contador_preciso.ipynb`](pruebas_contador_preciso.ipynb) | Lee `results_lock.csv` y grafica el tiempo vs. el número de hilos. |
| [`Makefile`](Makefile) | Compila los dos ejecutables. |

---

## El patrón lock → región crítica → unlock

Para hacer thread-safe una estructura de datos, las diapositivas (p. 4) proponen el patrón básico: agregar un lock a la estructura, adquirirlo antes de manipularla y liberarlo al terminar.

```c
typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

void increment(counter_t *c) {
    pthread_mutex_lock(&c->lock);     // 1. adquirir el lock
    c->value++;                       // 2. región crítica
    pthread_mutex_unlock(&c->lock);   // 3. liberar el lock
}
```

Mientras un hilo tiene el lock, los demás que llaman a `pthread_mutex_lock` esperan. Las tres instrucciones de `c->value++` ya no se pueden intercalar con las de otro hilo, así que no se pierden incrementos.

Las diapositivas escriben `Pthread_mutex_lock` (con mayúscula): son las macros de OSTEP que llaman a la función y verifican con `assert` que no falle. Están en [`../include/common_threads.h`](../include/common_threads.h). Este código usa directamente las funciones de la API.

---

## Compilación

```bash
make
```

Equivale a:

```bash
gcc -Wall -I../include main_precise_counter.c counter_lock.c -pthread -o precise_counter
gcc -Wall -I../include benchmark_precise_counter.c counter_lock.c -pthread -o benchmark_precise_counter
```

---

## Ejemplo 1 — `precise_counter` (16 hilos, un millón de incrementos cada uno)

```bash
./precise_counter
```

Salida de referencia:

```
-> Tiempo gastado: 1.406369 seg
-> El contador debe quedar en: 16000000
-> El valor real del contador es: 16000000
```

El valor es exacto en todas las ejecuciones (en dos ejecuciones más tardó 1.42 y 1.37 s). Ahora el valor es correcto, pero el tiempo es mucho mayor: el contador sin lock hace el mismo trabajo con 16 hilos en unos 0.17 s ([Parte 1](../counter_without_locks/README.md#experimento-variar-el-número-de-hilos)), aunque con un resultado incorrecto.

---

## Ejemplo 2 — `benchmark_precise_counter` (1 a 16 hilos)

```bash
./benchmark_precise_counter
```

Salida de referencia:

```
Iniciando benchmark para 'counter_lock'...
Resultados se guardarán en results_lock.csv
  Probando con 1 hilos...
    Tiempo: 0.0346 s, Esperado: 1000000, Real: 1000000
  Probando con 2 hilos...
    Tiempo: 0.1373 s, Esperado: 2000000, Real: 2000000
  Probando con 4 hilos...
    Tiempo: 0.2839 s, Esperado: 4000000, Real: 4000000
  Probando con 8 hilos...
    Tiempo: 0.6336 s, Esperado: 8000000, Real: 8000000
  Probando con 16 hilos...
    Tiempo: 1.3247 s, Esperado: 16000000, Real: 16000000
Benchmark completado.
```

El mismo contenido queda en [`results_lock.csv`](results_lock.csv). Se puede graficar con el notebook:

```bash
jupyter notebook pruebas_contador_preciso.ipynb
```

### Cómo leer los resultados

Con N hilos se hace N veces más trabajo (N millones de incrementos). Si los hilos trabajaran de verdad en paralelo, el tiempo se mantendría cerca del de un hilo: es el **escalamiento perfecto** (*perfect scaling*) de las diapositivas (p. 13). Aquí ocurre lo contrario: con 16 hilos el programa tarda unas 38 veces lo que tarda con uno.

El lock obliga a que los incrementos se hagan de a uno, así que agregar hilos no agrega paralelismo; además, cada vez que el lock pasa de un hilo a otro hay que esperar y coordinar entre núcleos, por lo que el tiempo crece incluso más rápido que el trabajo. Es la conclusión de las diapositivas (p. 11–13): **el contador sincronizado escala pobremente**.

| Hilos | Esta máquina (s) | Diapositivas, p. 11 (s) |
|---|---|---|
| 1 | 0.0346 | 0.0193 |
| 2 | 0.1373 | 0.0804 |
| 4 | 0.2839 | 0.1738 |
| 8 | 0.6336 | 0.3312 |
| 16 | 1.3247 | 0.6784 |

Los tiempos de las diapositivas son de otra ejecución, en otras condiciones de la máquina, y por eso no coinciden. La forma de la curva sí es la misma, y también la de la gráfica de OSTEP (diapositivas p. 12, Fig. 29.5), donde dos hilos ya tardan más de 5 s en un iMac de 4 núcleos.

---

## Limpieza

```bash
make clean
```

---

## Conclusión

El contador preciso resuelve la corrección, pero convierte un trabajo paralelizable en uno secuencial. La pregunta de las diapositivas (p. 13) es cómo lograr un contador que escale: el contador aproximado de la [Parte 3](../sloppy_counter/README.md).

---

## Referencias

- Arpaci-Dusseau, R. y Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*, v1.10. Cap. 29, *Lock-based Concurrent Data Structures*, §29.1 [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf), y cap. 27, *Interlude: Thread API*, §27.3 (locks) [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf).
- `man pthread_mutex_init`, `man pthread_mutex_lock`, `man pthread_mutex_unlock`.

> [!IMPORTANT]
> **Sobre esta guía.** El código y esta guía se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP; todos los programas se compilaron y ejecutaron para obtener las salidas de referencia. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente; se agradece reportarla en los foros del curso.
