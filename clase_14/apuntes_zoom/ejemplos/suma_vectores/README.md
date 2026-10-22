![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Suma de vectores serial y paralela: repaso rápido

Repaso del ejemplo de paralelismo de datos que se ejecutó en clase el 20/10: cómo compilarlo, cómo ejecutarlo y qué salida esperar. La explicación del concepto está en el [apunte de la clase](../../README.md#paralelizar); la guía completa, con el cálculo del *speedup*, las tablas de tiempos con distinto número de hilos y la versión en Python, está en [`clase_14/ejemplos/sum_vectores/`](../../../ejemplos/sum_vectores/README.md).

| Archivo | Contenido |
|---|---|
| [`suma_s.c`](suma_s.c) | Versión serial: un solo hilo suma `resultado[i] = v1[i] + v2[i]` para todo el vector. |
| [`suma_p.c`](suma_p.c) | Versión paralela: divide el vector en trozos contiguos, uno por hilo. Cada hilo recibe un `struct` con su identificador, los vectores y el tamaño; `main` los lanza con `pthread_create` y los espera con `pthread_join`. |
| [`common.h`](common.h) | `GetTime()`, que mide el tiempo con `gettimeofday`. |

Ambos programas usan vectores de 100 000 000 de enteros (unos 1.2 GB de RAM entre los tres arreglos) y miden **solo** la suma, no la reserva ni la inicialización de los vectores.

Requisitos: Linux (o WSL) con `gcc` y unos 1.2 GB de RAM libre (verifíquelo con `free -h`). Los comandos se ejecutan desde esta carpeta.

## Compilación

```sh
gcc -o suma_s suma_s.c -Wall
gcc -o suma_p suma_p.c -lpthread -Wall
```

`suma_s` no usa hilos, así que no necesita `-lpthread`.

## Ejecución

```sh
./suma_s
./suma_p              # un hilo por núcleo lógico
./suma_p 4            # o el número de hilos que se indique
```

> [!NOTE]
> **Sobre las salidas.** Son de una ejecución de referencia hecha el 22/10/2026 en la máquina del profesor (WSL1 sobre Windows 10, `gcc` 9.4.0, 8 CPU lógicas; ver [entorno de ejecución](../../../ejemplos/entorno_ejecucion.md)), no de la sesión de clase. Los tiempos cambian en cada ejecución y dependen de la máquina.

```
$ ./suma_s
Ejecutando la suma en modo serial...
Tiempo de ejecución serial (Ts): 0.583372 segundos.
$ ./suma_p
Usando 8 hilos para la suma.
Suma paralela completada.
Tiempo de ejecución paralela (Tp): 0.137615 segundos.
```

Tres ejecuciones seguidas de cada programa:

| Ejecución | Serial, $T_s$ (s) | Paralela con 8 hilos, $T_p$ (s) | $T_s / T_p$ |
|---|---|---|---|
| 1 | 0.583372 | 0.137615 | 4.2 |
| 2 | 0.652356 | 0.143344 | 4.6 |
| 3 | 0.604054 | 0.145552 | 4.2 |

En clase, en la misma máquina, la versión paralela con 8 hilos fue unas 3 veces más rápida (cerca de 0.47 s frente a 0.15 s). En ambos casos el resultado es del mismo orden: varias veces más rápida, pero no 8 veces, aunque haya 8 hilos. La razón es que la máquina tiene 4 núcleos físicos y 8 lógicos: compilado sin optimización (como aquí), el trabajo está limitado por la CPU y escala hasta los 4 núcleos físicos; los 4 hilos extra corren en núcleos lógicos que comparten unidades de ejecución con los primeros y aportan poco. La [guía de C](../../../ejemplos/sum_vectores/c/README.md#resultados-en-la-máquina-de-prueba) mide 1, 2, 4 y 8 hilos, y muestra que con `-O2` el límite pasa a ser la memoria.

## Para analizar

Identifique en `suma_p.c`:

1. Dónde se define el `struct` de argumentos y qué campos usa cada hilo para calcular su trozo (`inicio` y `fin`).
2. Por qué el último hilo usa `fin = tamano_total`.
3. Dónde se crean los hilos y dónde se espera a que terminen. ¿Qué pasaría con la medición si se quitaran los `pthread_join`?
4. Si hay alguna variable que dos hilos escriban a la vez. ¿Puede haber una *race condition* como la de `t1`?

## Limpieza

```sh
rm -f suma_s suma_p
```
