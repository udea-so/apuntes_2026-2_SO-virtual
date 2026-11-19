# Problema productor-consumidor con buffer circular (Solución)

> [!Important]
> Esta implentación soluciona los problemas del [**pc_attemp1**](../pc_attemp1/README.md) y del  [**pc_attemp2**](../pc_attemp2/README.md)

## Descripción General

Este proyecto (`pc_work`) presenta la implementación correcta y robusta del problema del Productor-Consumidor utilizando un Buffer Circular. Esta solución integra los mecanismos de sincronización necesarios para garantizar la integridad de los datos y la fluidez de la ejecución concurrente.

A diferencia de los intentos anteriores, esta versión gestiona adecuadamente el alcance (scope) de los cerrojos, resolviendo las **Condiciones de Carrera** sobre los índices del arreglo y evitando el **Interbloqueo (Deadlock)** al ordenar correctamente las esperas.

## Arquitectura de la Solución

El diseño desacopla la espera por recursos (espacio/datos) de la protección de la sección crítica (memoria compartida).

```mermaid
graph TD
    subgraph Productor
    P_W[1. Esperar Espacio <br/> sem_wait empty] --> P_L[2. Adquirir Mutex <br/> lock mutex]
    P_L --> P_ACT[3. Escribir Dato <br/> put]
    P_ACT --> P_U[4. Liberar Mutex <br/> unlock mutex]
    P_U --> P_S[5. Señalar Dato <br/> sem_post full]
    end

    subgraph Consumidor
    C_W[1. Esperar Dato <br/> sem_wait full] --> C_L[2. Adquirir Mutex <br/> lock mutex]
    C_L --> C_ACT[3. Leer Dato <br/> get]
    C_ACT --> C_U[4. Liberar Mutex <br/> unlock mutex]
    C_U --> C_S[5. Señalar Espacio <br/> sem_post empty]
    end
```

## Estructura del Proyecto

  * **`pc_circular.c`**: Código fuente con la lógica de sincronización corregida.
  * **`common.h` / `common_threads.h`**: Encabezados auxiliares y macros de manejo de hilos.
  * **`Makefile`**: Script de compilación automatizada.

## Mecanismo de Sincronización

La solución emplea una estrategia combinada de **Semáforos Contadores** y **Exclusión Mutua (Mutex)**, asignando a cada uno una responsabilidad específica:

1.  **Semáforos (`empty`, `full`)**: Gestionan la **Orquestación**.
      * Determinan *cuándo* un hilo debe dormir porque no puede proceder lógicamente (buffer lleno o vacío).
2.  **Mutex (`mutex`)**: Gestiona la **Integridad**.
      * Garantiza que la manipulación de los índices `fill` y `use` (y la escritura/lectura en el array) sea atómica.

## Lógica de Ejecución (La Corrección)

El cambio fundamental respecto al *Intento 2* es la **reubicación de las llamadas de espera**.

### Protocolo del Productor

```c
// Lógica Correcta
sem_wait(&empty);       // (P1) Esperar espacio (SIN tener el mutex)
sem_wait(&mutex);       // (P2) Proteger índices
put(i);                 // (P3) Sección Crítica
sem_post(&mutex);       // (P4) Liberar protección
sem_post(&full);        // (P5) Avisar al consumidor
```

### Protocolo del Consumidor

```c
// Lógica Correcta
sem_wait(&full);        // (C1) Esperar dato (SIN tener el mutex)
sem_wait(&mutex);       // (C2) Proteger índices
get();                  // (C3) Sección Crítica
sem_post(&mutex);       // (C4) Liberar protección
sem_post(&empty);       // (C5) Avisar al productor
```

### Análisis de Robustez

1.  **Prevención de Race Condition**: Al envolver `put()` y `get()` entre `wait(&mutex)` y `post(&mutex)`, aseguramos que solo un hilo modifique los índices a la vez.
2.  **Prevención de Deadlock**: Al realizar el `sem_wait(&empty/full)` **antes** de adquirir el mutex, si un hilo se duerme (por ejemplo, buffer lleno), **no retiene el mutex**. Esto permite que el hilo contrario (el consumidor) pueda adquirir el mutex, consumir un elemento y liberar espacio, despertando al productor.

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

Para validar la corrección de esta solución:

1.  **Prueba de Carga**: Configure `producers = 5` y `consumers = 5` en el `main`. Ejecute con un número alto de loops. El programa debe terminar correctamente sin bloquearse ni corromper datos.
2.  **Verificación de Salida**: Puede redirigir la salida a un archivo y utilizar comandos como `sort` y `uniq` para verificar que todos los números producidos fueron consumidos exactamente una vez.
3.  **Comparativa**: Compare el tiempo de ejecución de esta solución correcta con una versión donde la sección crítica sea innecesariamente grande. Esto demostrará la importancia de minimizar la granularidad del bloqueo (Fine-grained locking).

> [!NOTE]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality.

