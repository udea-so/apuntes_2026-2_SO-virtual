# Sistemas Operativos — Clase 14: Ejemplos

Programas de ejemplo de la clase de hilos (OSTEP, capítulos 26 y 27). Cada carpeta tiene su propia guía, con compilación, salidas de referencia y preguntas para probar cambios. El orden sugerido para recorrerlas está en la [ruta de lectura](../README.md#ruta-de-lectura) de la clase.

| Carpeta / archivo | Contenido | OSTEP |
|---|---|---|
| [`api-thread/`](api-thread/README.md) | `t0`–`t3` en C y en Python: creación de hilos, orden no determinista, datos compartidos y race condition, paso de argumentos y valores de retorno | Cap. 26 (§26.2–26.4) y cap. 27 (§27.1–27.3) |
| [`api-thread/anexo_c_python.md`](api-thread/anexo_c_python.md) | Comparación lado a lado de las versiones en C y en Python: mismo concepto, distinta API | — |
| [`sum_vectores/`](sum_vectores/README.md) | Paralelismo de datos: suma de dos arreglos en versión serial y paralela, en [C](sum_vectores/c/README.md) y en [Python](sum_vectores/python/README.md); medición del speedup, por qué depende de si el límite es la CPU, la memoria o el GIL, y comparación entre ambos lenguajes | Cap. 26 (§26.1) |
| [`entorno_ejecucion.md`](entorno_ejecucion.md) | Máquina donde se obtuvieron las salidas de las guías y qué cambia al ejecutar en WSL, máquinas virtuales, Docker o Linux nativo | — |

La tarea de simulación del capítulo 26 (`x86.py`) está en la carpeta [`simulacion/`](../simulacion/README.md) de la clase.
