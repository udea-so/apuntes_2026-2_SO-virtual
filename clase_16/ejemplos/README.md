# Ejemplos — Clase 16: estructuras de datos basadas en locks

Ejemplos en C de estructuras de datos concurrentes protegidas con locks, basados en el capítulo 29 de OSTEP (*Lock-based Concurrent Data Structures*). Cada carpeta tiene su propio `README.md` con la explicación del código, las instrucciones de compilación y salidas de una ejecución de referencia.

| Estructura | Carpeta | Diapositivas | OSTEP |
|---|---|---|---|
| Contador | [`counters/`](counters/README.md): sin lock, preciso, aproximado (*sloppy*) y comparación | p. 4–25 | §29.1 |
| Lista enlazada | [`lists/`](lists/README.md): sin lock y con un lock para toda la lista | p. 26–35 | §29.2 |

La cola concurrente (p. 37–42, OSTEP §29.3) y la tabla hash concurrente (p. 43–45, OSTEP §29.4) no tienen ejemplos en esta carpeta.

Requisitos: Linux (nativo, WSL, máquina virtual o contenedor) con `gcc` y `make`. Las salidas de referencia se obtuvieron en la máquina descrita en el [entorno de ejecución](../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14.
