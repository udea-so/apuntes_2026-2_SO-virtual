# Experimento de Comparación de Contadores (Preciso vs. Aproximado)

Este documento describe el procedimiento para compilar y ejecutar un experimento de *benchmarking* que compara el rendimiento de dos implementaciones de contadores en un entorno concurrente.

## Propósito

El objetivo de este experimento es observar y cuantificar la sobrecarga (overhead) generada por la **contención de bloqueos** en un contador preciso (que utiliza un único *mutex* global) y demostrar cómo la implementación de un **contador aproximado** (*sloppy counter*), que utiliza bloqueos locales, mejora sigificativamente el rendimiento en sistemas multinúcleo.

## 1. Nota Importante sobre `common.h`

El archivo `benchmark_counters.c` depende de un archivo de cabecera llamado `common.h`, el cual debe proveer una función `GetTime()`.

Este archivo no se encuentra entre los ficheros proporcionados, pero el comando de compilación (sugerido en el código fuente) espera encontrarlo en un directorio `include` ubicado un nivel por encima de donde se compila.

**Acción Requerida**: Antes de compilar, asegúrese de tener el archivo `common.h` (provisto por el curso) y ubíquelo en la ruta `../include/common.h` relativa a su directorio de trabajo.

## 2. Compilación y Ejecución del Benchmark

Siga estos pasos para ejecutar el experimento.

### Paso 2.1: Compilación

Coloque todos los archivos fuente (`.c`) y de cabecera (`.h`) proporcionados en un único directorio. Abra una terminal en ese directorio y ejecute el siguiente comando para compilar el programa:

```bash
gcc -Wall -I../include precise_counter.c sloppy_counter.c benchmark_counters.c -lpthread -o run_benchmark
```

  * `-Wall`: Habilita todas las advertencias del compilador.
  * `-I../include`: Indica al compilador que busque `common.h` en el directorio padre `include/`.
  * `-lpthread`: Enlaza la biblioteca POSIX Threads, necesaria para `pthread_mutex_t` y la creación de hilos.
  * `-o run_benchmark`: Nombra el archivo ejecutable de salida como `run_benchmark`.

### Paso 2.2: Ejecución

Una vez compilado exitosamente, ejecute el *benchmark* con el siguiente comando:

```bash
./run_benchmark
```

El programa ejecutará las pruebas para ambos tipos de contadores (Preciso y Aproximado) utilizando un número creciente de hilos (definido en `benchmark_counters.c`).

Al finalizar, imprimirá un mensaje de confirmación y habrá generado un nuevo archivo llamado `results.csv` en el mismo directorio.


## 3. Análisis y Visualización de Resultados

El archivo `results.csv` contiene los datos de tiempo de ejecución para cada tipo de contador y número de hilos.

Para visualizar estos resultados gráficamente:

1.  Asegúrese de tener `results.csv` en el mismo directorio que el archivo `plot_results.ipynb`.
2.  Inicie su entorno de Jupyter (Jupyter Lab o Jupyter Notebook).
3.  Abra el *notebook* `plot_results.ipynb`.
4.  Ejecute todas las celdas.

El *notebook* leerá el archivo `results.csv`, generará una gráfica comparativa y mostrará una tabla con los datos de rendimiento. Esta gráfica es la herramienta principal para analizar la escalabilidad de cada implementación.

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Tenga en cuenta que puede contener errores.

> [!warning]
> **Aclaración**: Como todo código (especialmente los ejemplos didácticos diseñados pueden fallar (o de hecho fallan)), el contenido debe ser revisado críticamente y puede contener errores.