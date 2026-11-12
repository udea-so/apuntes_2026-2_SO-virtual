![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Contadores concurrentes

Ejemplos del contador concurrente de las diapositivas (p. 4–25) y de OSTEP §29.1 (*Concurrent Counters*). Están organizados como una cadena: cada parte resuelve el problema de la anterior y deja uno nuevo.

| Parte | Carpeta | Diapositivas | OSTEP | Qué se observa |
|---|---|---|---|---|
| 1 | [`counter_without_locks/`](counter_without_locks/README.md) | p. 5–7 | Fig. 29.1 | Sin lock, varios hilos pierden la mayoría de los incrementos (race condition) |
| 2 | [`precise_counter/`](precise_counter/README.md) | p. 9–13 | Figs. 29.2 y 29.5 | Con un lock el valor es exacto, pero el tiempo crece con el número de hilos: escala pobremente |
| 3 | [`sloppy_counter/`](sloppy_counter/README.md) | p. 14–25 | Figs. 29.3–29.6 | El contador aproximado tal como en las diapositivas: el valor queda por debajo del real según el umbral S, y en la máquina de referencia casi no mejora frente al preciso |
| 4 | [`comparison/`](comparison/README.md) | p. 12, 24 y 25 | Figs. 29.5 y 29.6 | El mismo contador aproximado con cada contador local en su propia línea de caché: reproduce las gráficas de OSTEP |

La parte 4 va más allá de las diapositivas: explica por qué el contador aproximado de la parte 3 no escala en la máquina de referencia (*false sharing*) y cómo se corrige.

Las cuatro partes usan los mismos parámetros (un millón de incrementos por hilo; S = 1024 al variar los hilos; 4 hilos al variar S), así que sus resultados se pueden comparar directamente. Las salidas de referencia son del 11/11/2026, en la máquina descrita en el [entorno de ejecución](../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14.

---

## Compilación

Se necesita Linux (nativo, WSL, máquina virtual o contenedor) con `gcc` y `make`. Cada carpeta tiene su `Makefile`:

```bash
cd counter_without_locks   # o precise_counter, sloppy_counter, comparison
make                       # compila los ejecutables de la carpeta
make clean                 # los elimina
```

Los programas incluyen [`include/common.h`](include/common.h), que define `GetTime()` (tomado de OSTEP); por eso los `Makefile` compilan con `-I../include`. [`include/common_threads.h`](include/common_threads.h) tiene las macros de OSTEP (`Pthread_mutex_lock`, …) que aparecen en las diapositivas; los ejemplos usan directamente las funciones de la API.

Los notebooks (`.ipynb`) son opcionales: grafican los `.csv` de cada carpeta y necesitan Python 3 con `pandas`, `matplotlib` y Jupyter.

---

## Resumen

| Contador | Valor | Desempeño |
|---|---|---|
| Sin lock | Incorrecto con más de un hilo | Rápido |
| Preciso (un lock) | Exacto | Escala pobremente |
| Aproximado (*sloppy*) | Por debajo del real hasta en `NUMCPUS × (S − 1)` | Escala bien si los contadores locales no comparten línea de caché |

> [!IMPORTANT]
> **Sobre estas guías.** El código y las guías se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP; todos los programas se compilaron y ejecutaron para obtener las salidas de referencia. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente; se agradece reportarla en los foros del curso.
