![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Contadores — Parte 4: preciso vs. aproximado sin *false sharing*

Esta carpeta repite, en un solo programa y con los mismos parámetros, los dos experimentos de OSTEP que aparecen en las diapositivas:

1. **Preciso vs. aproximado** al variar el número de hilos (diapositivas p. 12 y 24, OSTEP Fig. 29.5).
2. **Aproximado al variar el umbral S** con 4 hilos (diapositivas p. 25, OSTEP Fig. 29.6).

El contador aproximado de esta carpeta tiene la **misma lógica** que el de la [Parte 3](../sloppy_counter/README.md) (el de las diapositivas). Lo único que cambia es la distribución en memoria: cada contador local y su lock ocupan su propia línea de caché, lo que evita el *false sharing* explicado en la [Parte 3](../sloppy_counter/README.md#por-qué-no-escala-como-en-ostep). Con ese cambio, los resultados se parecen a los de OSTEP.

Las salidas son de una ejecución de referencia del 11/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14 (WSL1, `gcc` 9.4.0, AMD Ryzen 5 3500U con 4 núcleos físicos y 8 lógicos).

---

## Archivos

| Archivo | Contenido |
|---|---|
| [`precise_counter.h`](precise_counter.h) / [`precise_counter.c`](precise_counter.c) | El contador preciso de la [Parte 2](../precise_counter/README.md), con las funciones renombradas (`precise_init`, `precise_increment`, …) para poder enlazarlo junto al aproximado en el mismo programa. |
| [`sloppy_counter.h`](sloppy_counter.h) / [`sloppy_counter.c`](sloppy_counter.c) | El contador aproximado con cada contador local en su propia línea de caché (funciones `sloppy_init`, `sloppy_update`, `sloppy_get`). |
| [`benchmark_counters.c`](benchmark_counters.c) | Ejecuta los dos experimentos y guarda `results.csv` y `results_threshold.csv`. Genera `run_benchmark`. |
| [`results.csv`](results.csv) / [`results_threshold.csv`](results_threshold.csv) | Resultados de la ejecución de referencia. |
| [`comparacion_contadores.ipynb`](comparacion_contadores.ipynb) | Grafica los dos experimentos junto con los de la versión de las diapositivas (`../sloppy_counter/*.csv`). |
| [`Makefile`](Makefile) | Compila `run_benchmark`. |

---

## El único cambio: la distribución en memoria

Versión de las diapositivas ([`../sloppy_counter/sloppy_counter.h`](../sloppy_counter/sloppy_counter.h)):

```c
typedef struct __counter_t {
  int global;
  pthread_mutex_t glock;
  int local[NUMCPUS];               // los 8 contadores, uno junto al otro
  pthread_mutex_t llock[NUMCPUS];   // los 8 locks, uno junto al otro
  int threshold;
} counter_t;
```

Versión de esta carpeta ([`sloppy_counter.h`](sloppy_counter.h)):

```c
typedef struct __local_counter_t {
    int value;                 // contador local
    pthread_mutex_t lock;      // su lock
} __attribute__((aligned(64))) local_counter_t;   // ocupa una línea de caché completa

typedef struct __sloppy_counter_t {
    int global;
    pthread_mutex_t glock;
    int threshold;
    local_counter_t local[NUMCPUS];   // cada contador local en su propia línea
} sloppy_counter_t;
```

`__attribute__((aligned(64)))` es una extensión de `gcc` que obliga a que cada `local_counter_t` empiece en una dirección múltiplo de 64 y ocupe 64 bytes: así dos contadores locales nunca comparten una línea de caché. En `update()` solo cambia la forma de acceder a los campos: `c->local[i]` pasa a ser `c->local[i].value` y `c->llock[i]` pasa a ser `c->local[i].lock`.

---

## Compilación y ejecución

```bash
make
./run_benchmark
```

`make` equivale a:

```bash
gcc -Wall -I../include benchmark_counters.c precise_counter.c sloppy_counter.c -pthread -o run_benchmark
```

Salida de referencia:

```
Experimento 1: preciso vs. aproximado (S=1024, 1000000 incrementos por hilo)
   1 hilos  Preciso:     0.0334 s  (valor: 1000000 de 1000000)
   1 hilos  Aproximado:  0.0400 s  (valor: 999424 de 1000000)
   2 hilos  Preciso:     0.1342 s  (valor: 2000000 de 2000000)
   2 hilos  Aproximado:  0.0409 s  (valor: 1998848 de 2000000)
   4 hilos  Preciso:     0.2760 s  (valor: 4000000 de 4000000)
   4 hilos  Aproximado:  0.0417 s  (valor: 3997696 de 4000000)
   8 hilos  Preciso:     0.6339 s  (valor: 8000000 de 8000000)
   8 hilos  Aproximado:  0.0569 s  (valor: 7995392 de 8000000)
  16 hilos  Preciso:     1.2932 s  (valor: 16000000 de 16000000)
  16 hilos  Aproximado:  0.1510 s  (valor: 15998976 de 16000000)

Experimento 2: aproximado con 4 hilos, variando S
  S=1      0.5714 s  (valor: 4000000 de 4000000)
  S=2      0.3238 s  (valor: 4000000 de 4000000)
  S=4      0.2276 s  (valor: 4000000 de 4000000)
  S=8      0.1602 s  (valor: 4000000 de 4000000)
  S=16     0.1186 s  (valor: 4000000 de 4000000)
  S=32     0.0773 s  (valor: 4000000 de 4000000)
  S=64     0.0583 s  (valor: 4000000 de 4000000)
  S=128    0.0529 s  (valor: 3999744 de 4000000)
  S=256    0.0536 s  (valor: 3999744 de 4000000)
  S=512    0.0440 s  (valor: 3999744 de 4000000)
  S=1024   0.0464 s  (valor: 3997696 de 4000000)

Benchmark completado. Resultados en results.csv y results_threshold.csv
```

Los valores del contador aproximado son los mismos de la [Parte 3](../sloppy_counter/README.md) (la lógica es idéntica): lo que falta es lo que quedó en los contadores locales sin llegar al umbral, `(incrementos que recibió cada local) mod S`.

Para ver las gráficas:

```bash
jupyter notebook comparacion_contadores.ipynb
```

---

## Resultados

### Experimento 1: tiempo vs. número de hilos (S = 1024)

Las tres implementaciones, medidas con los mismos parámetros (un millón de incrementos por hilo):

| Hilos | Preciso (s) | Aproximado, diapositivas (s) | Aproximado sin false sharing (s) |
|---|---|---|---|
| 1 | 0.0334 | 0.0406 | 0.0400 |
| 2 | 0.1342 | 0.1524 | 0.0409 |
| 4 | 0.2760 | 0.2620 | 0.0417 |
| 8 | 0.6339 | 0.4399 | 0.0569 |
| 16 | 1.2932 | 0.9105 | 0.1510 |

La columna de la versión de las diapositivas viene de [`../sloppy_counter/results_sloppy.csv`](../sloppy_counter/results_sloppy.csv); las otras dos, de [`results.csv`](results.csv).

- **Hasta 4 hilos** (los núcleos físicos de la máquina), el contador aproximado sin false sharing tarda prácticamente lo mismo aunque el trabajo total se multiplique por 4: es el escalamiento casi perfecto de la gráfica de OSTEP. Con 4 hilos es unas 6.6 veces más rápido que el preciso.
- **Con 8 hilos** hay dos hilos por núcleo físico (*hyperthreading*) y el tiempo sube un poco; **con 16** hay además dos hilos por contador local, que compiten por su lock.
- **La versión de las diapositivas**, con la misma lógica, se queda cerca del contador preciso.

### Experimento 2: tiempo vs. S (4 hilos)

Con S = 1 el contador es lento (cada incremento toma también el lock global) y su valor es exacto. A medida que S crece, el tiempo cae (de 0.57 s a unos 0.05 s) y deja de mejorar de forma apreciable a partir de S ≈ 64–128, como en la gráfica de OSTEP; a cambio, `get()` se retrasa más respecto al valor real. Con la versión de las diapositivas, en cambio, el tiempo deja de bajar a partir de S = 4 (ver el notebook, que grafica las dos curvas juntas).

Los tiempos absolutos son mucho menores que los de OSTEP (por ejemplo, el preciso con 2 hilos: 0.13 s aquí, más de 5 s en OSTEP) porque la máquina y la implementación de los mutex son otras. Lo que se compara es la forma de las curvas.

---

## Limpieza

```bash
make clean
```

---

## Conclusiones

| Contador | Valor | Desempeño en la máquina de referencia |
|---|---|---|
| [Sin lock](../counter_without_locks/README.md) | Incorrecto con más de un hilo | Rápido |
| [Preciso](../precise_counter/README.md) | Exacto | Escala pobremente: el tiempo crece con los hilos |
| [Aproximado, versión de las diapositivas](../sloppy_counter/README.md) | Aproximado (por debajo hasta en `NUMCPUS × (S − 1)`) | Apenas mejor que el preciso, por *false sharing* |
| Aproximado sin false sharing (esta carpeta) | Aproximado (igual al anterior) | Escala casi perfectamente hasta 4 hilos |

El contador aproximado resuelve el problema de desempeño del contador preciso, como muestran las diapositivas. La lección adicional de esta carpeta es que en un multiprocesador no basta con dar a cada hilo su propio lock: los datos que cada hilo modifica deben estar también en líneas de caché distintas.

---

## Referencias

- Arpaci-Dusseau, R. y Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*, v1.10. Cap. 29, *Lock-based Concurrent Data Structures*, §29.1, Figs. 29.5 y 29.6 [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf), y cap. 10, *Multiprocessor Scheduling (Advanced)*, §10.1 (coherencia de caché) [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-multi.pdf).
- Documentación del kernel de Linux: *False Sharing* [[link]](https://docs.kernel.org/kernel-hacking/false-sharing.html).
- Documentación de `gcc`: *Common Variable Attributes* (`aligned`) [[link]](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html).

> [!IMPORTANT]
> **Sobre esta guía.** El código y esta guía se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP; todos los programas se compilaron y ejecutaron para obtener las salidas de referencia. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente; se agradece reportarla en los foros del curso.
