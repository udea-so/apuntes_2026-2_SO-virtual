# Problema del Productor-Consumidor: Buffer Circular (Intento 1 - Sin Exclusión Mutua)

## Descripción General

Este proyecto representa una evolución del problema clásico del Productor-Consumidor. A diferencia de la versión de buffer único ($N=1$), aquí se implementa un **Buffer Circular** (un arreglo de tamaño $N > 1$) para permitir un mayor desacoplamiento entre la producción y el consumo.

Esta implementación (`pc_attemp1`) utiliza semáforos para coordinar si el buffer está lleno o vacío. Sin embargo, **esta versión contiene un defecto crítico de concurrencia** intencional para fines educativos: carece de protección para los índices del arreglo, lo que provoca **Condiciones de Carrera** cuando existen múltiples productores o consumidores.

## Arquitectura del Sistema

El sistema ahora modela una cola circular donde múltiples elementos pueden residir en memoria simultáneamente.

```mermaid
graph LR
    subgraph Productores
    P1((Prod 1))
    P2((Prod 2))
    end

    subgraph Recurso Compartido
    B[Buffer Circular (Array)]
    end

    subgraph Consumidores
    C1((Cons 1))
    end

    P1 -- put(fill) --> B
    P2 -- put(fill) --> B
    B -- get(use) --> C1

    note[Fallo: Race Condition en índices 'fill' y 'use'] -.-> B
```

## Estructura del Proyecto

  * **`pc_circular.c`** (o el nombre de su archivo fuente): Código que implementa la lógica del buffer circular.
  * **`common.h` / `common_threads.h`**: Librerías auxiliares.
  * **`Makefile`**: Script de compilación.

## Mecanismo de Sincronización

Se emplean semáforos contadores para gestionar la capacidad del buffer.

### Variables de Estado

1.  **Buffer (`int buffer[MAX]`)**: Arreglo compartido de tamaño `MAX`.
2.  **Índices**:
      * `fill`: Apunta a la posición donde el productor escribirá el siguiente dato.
      * `use`: Apunta a la posición donde el consumidor leerá el siguiente dato.
3.  **Semáforos**:
      * `empty` (Inicializado en `MAX`): Cuenta los espacios libres.
      * `full` (Inicializado en `0`): Cuenta los espacios ocupados (items listos).

## Lógica de Ejecución (Y el Problema)

### Rutina del Productor (`put`)

El productor intenta escribir un dato y avanzar el índice `fill` de manera circular:

```c
buffer[fill] = value;       // (F1) Escribir dato
fill = (fill + 1) % MAX;    // (F2) Actualizar índice
```

### Análisis del Fallo: La Condición de Carrera

Aunque los semáforos `empty` y `full` aseguran que no escribamos en un buffer lleno ni leamos de uno vacío, **no protegen la integridad de los índices `fill` y `use`**.

Si tenemos **2 Productores** y el buffer tiene espacio (`empty >= 2`), ambos pueden decrementar el semáforo `empty` y entrar a la función `put` **simultáneamente**.

#### Traza de Ejecución Fallida (Ejemplo)

Suponga `fill = 0` y `MAX = 10`.

1.  **Productor A** ejecuta `sem_wait(&empty)`. Entra.
2.  **Productor B** ejecuta `sem_wait(&empty)`. Entra (intercalación).
3.  **Productor A** escribe en `buffer[0]` el valor `X` (Línea F1).
4.  **[Context Switch]** El sistema pausa a A y ejecuta a B.
5.  **Productor B** lee `fill` (que sigue siendo 0).
6.  **Productor B** escribe en `buffer[0]` el valor `Y`. **¡El valor `X` ha sido sobrescrito y perdido\!**
7.  **Productor B** actualiza `fill` a 1 (Línea F2).
8.  **[Context Switch]** Vuelve Productor A.
9.  **Productor A** retoma su ejecución. Tenía una copia antigua de `fill` (0) o lee el nuevo (1) dependiendo de la compilación, corrompiendo la secuencia lógica del buffer.

**Diagnóstico**: Las líneas F1 y F2 constituyen una **Sección Crítica** que no está protegida por Exclusión Mutua (Mutex).

## Compilación y Ejecución

1.  **Compilar**:

    ```bash
    make
    ```

2.  **Ejecutar**:

    ```bash
    ./pc_circular
    ```

## Actividades Académicas

Para demostrar el error de concurrencia:

1.  **Prueba de Estrés**: Configure `producers = 2` y un número alto de `loops` (ej. 100,000).
2.  **Verificación**: Redirija la salida a un archivo y verifique si faltan números en la secuencia consumida.
    ```bash
    ./pc_circular > salida.txt
    # Busque saltos o duplicados en salida.txt
    ```
3.  **Solución Teórica**: Antes de pasar a la siguiente implementación, discuta: ¿Dónde debería colocarse un `pthread_mutex_lock` para solucionar esto? ¿Qué riesgos de *Deadlock* introduce ese nuevo lock?

> [!NOTE]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality.


