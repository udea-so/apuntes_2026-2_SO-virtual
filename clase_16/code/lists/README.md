# Listas Enlazadas y Concurrencia (OSTEP Cap. 29)

Este directorio contiene ejemplos prácticos sobre **Estructuras de Datos Concurrentes**, basados en el Capítulo 29 del libro *Operating Systems: Three Easy Pieces* (Remzi & Andrea Arpaci-Dusseau).

El objetivo es entender cómo los hilos (threads) interactúan con estructuras de datos compartidas y los peligros de no sincronizar el acceso a la memoria.

## Organización del Código

El ejercicio se divide en dos etapas fundamentales:

### 1. [`sin_locks/`](sin_locks/) (El Problema)
* **Qué es:** Una implementación ingenua de una lista enlazada estándar.
* **El objetivo:** Evidenciar **Condiciones de Carrera (Race Conditions)**. Al ejecutar este código con múltiples hilos, verás cómo se pierden datos (nodos) y se corrompe la memoria debido a la falta de protección.

### 2. [`con_locks/`](./con_locks/) (La Solución)
* **Qué es:** La misma lista enlazada, pero protegida mediante **Mutexes** (`pthread_mutex_t`).
* **El objetivo:** Demostrar cómo el bloqueo (locking) garantiza la **Exclusión Mutua** y la integridad de los datos, convirtiendo la estructura en *Thread-Safe*.


## Mapa Mental del Ejercicio

Este diagrama resume el flujo de aprendizaje que debes seguir con estos ejemplos:

```mermaid
graph TD
    Start([Inicio: Listas en SO]) --> A[Carpeta: sin_locks]
    A -->|Ejecutar con Hilos| B{¿Resultado?}
    B -->|Datos Perdidos| C[Fallo: Race Condition]
    C -->|Necesitamos Sincronización| D[Carpeta: con_locks]
    
    D -->|Implementar Mutex| E[Lock -> Sección Crítica -> Unlock]
    E -->|Ejecutar con Hilos| F{¿Resultado?}
    F -->|Datos Integros| G([Éxito: Thread-Safe])
    
    style C fill:#ffcccc,stroke:#ff0000
    style G fill:#ccffcc,stroke:#00aa00
````

## Instrucciones Rápidas

1.  Dirígete primero a la carpeta [sin_locks](./sin_locks/).
2.  Compila y ejecuta el código intentando insertar miles de nodos con varios hilos. Observa que el conteo final falla.
3.  Luego, ve a la carpeta [con_locks](./con_locks/).
4.  Analiza cómo se usan `pthread_mutex_lock` y `unlock` en `list.c`.
5.  Compila y verifica que ahora el conteo es exacto.

> **Referencia Teórica:** [OSTEP Chapter 29: Lock-based Concurrent Data Structures](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf)

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Puede contener errores.
