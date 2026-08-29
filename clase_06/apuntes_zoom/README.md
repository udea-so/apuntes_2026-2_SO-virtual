![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Planificador MLFQ (Multi-Level Feedback Queue)

## Objetivos de Aprendizaje

* **Explicar**: Por qué se necesita un planificador de propósito general (MLFQ) que no dependa de conocer de antemano el tiempo de ejecución de los procesos.
* **Aplicar**: Las cinco reglas del MLFQ (incluida la Nueva Regla 4) para determinar el movimiento de un proceso entre colas de prioridad.
* **Identificar**: Los problemas de inanición (*starvation*) y de engaño al planificador (*gaming the scheduler*) presentes en el MLFQ clásico.
* **Analizar**: Trazas de ejecución del simulador `mlfq.py` para procesos CPU-bound, cortos e I/O-bound.
* **Comparar**: El efecto de variar el tamaño del quantum por cola sobre el tiempo de respuesta y el turnaround time.

## Tipos de Procesos

Se distinguen dos tipos básicos de procesos según su patrón de uso de CPU e I/O:

* **Procesos tipo batch (*CPU-bound*)**: ráfagas largas de CPU intercaladas con esperas de I/O poco frecuentes. Les preocupa principalmente el **Turnaround Time**.
* **Procesos interactivos (*I/O-bound*)**: ráfagas cortas de CPU con operaciones de I/O frecuentes. Les preocupa principalmente el **Response Time**.

## Hacia un Planificador de Propósito General

Al relajar la última suposición ideal de planificación (que el tiempo de ejecución de cada trabajo es conocido), se concluye que el *scheduler* **no conoce nada** para decidir cuál será el próximo proceso en ejecutarse. Esto plantea dos preguntas clave:

* ¿Cómo llevar a cabo la planificación sin tener un conocimiento perfecto de la situación?
* ¿Cómo diseñar un **planificador de propósito general** que funcione bien tanto para procesos interactivos como para procesos tipo batch?

La propuesta para resolver ambas preguntas es el **MLFQ (Multi-Level Feedback Queue)**: un planificador que aprende del comportamiento pasado de los procesos para predecir su comportamiento futuro.

## MLFQ: Conceptos y Reglas Básicas

El MLFQ organiza los procesos en **varias colas**, cada una con su propio **nivel de prioridad**. Un proceso listo se ubica en una de las colas, y esa prioridad puede cambiar con el tiempo según el **comportamiento observado** (no se usa un valor fijo).

* **Alta prioridad**: procesos I/O-bound.
* **Baja prioridad**: procesos CPU-bound.

En términos de tipo de proceso, la jerarquía de prioridades típica es: procesos de tiempo real (más alta) → procesos del sistema operativo → procesos interactivos → procesos tipo batch (más baja).

Las reglas iniciales (versión clásica) del MLFQ son:

* **Regla 1**: Si prioridad(A) > prioridad(B), se ejecuta A (y no B).
* **Regla 2**: Si prioridad(A) = prioridad(B), se aplica Round Robin entre A y B.
* **Regla 3**: Cuando un trabajo llega al sistema, se ubica en la cola de mayor prioridad.
* **Regla 4a**: Si el trabajo usa completamente el quantum de tiempo, se reduce su prioridad (baja una cola).
* **Regla 4b**: Si el trabajo entrega la CPU antes de finalizar su quantum, mantiene el mismo nivel de prioridad.
* **Regla 5**: Después de un tiempo **S**, todos los trabajos se mueven a la cola de mayor prioridad.

> [!Note]
> Continuación de la sesión del 27/08/2026 — a partir de aquí se profundizó en cada regla, se ejecutaron ejemplos con el simulador `mlfq.py` y se presentaron los problemas y mejoras del MLFQ.

## Reglas 1 y 2: Prioridad entre Colas y Round Robin

Dado un conjunto de colas Q3 > Q2 > Q1 > Q0 (de mayor a menor prioridad), si un proceso A está en Q3 y B en Q2, A se ejecuta primero (Regla 1). Si dos procesos están en la misma cola (por ejemplo, C y D en Q0), se alternan mediante Round Robin (Regla 2) — en el ejemplo del manuscrito, si prioridad(D) == prioridad(C), ambos se reparten la CPU por turnos dentro de esa cola.

## Reglas 3, 4a y 4b: Ingreso y Cambio de Prioridad

Todo proceso nuevo entra siempre a la cola de mayor prioridad (Regla 3). A partir de ahí, su prioridad desciende una cola cada vez que agota por completo su quantum de tiempo (Regla 4a), y se mantiene en la misma cola si libera la CPU voluntariamente antes de que el quantum expire (Regla 4b, típicamente por una operación de I/O).

## Ejemplos con el Simulador MLFQ

Los siguientes ejemplos usan una MLFQ de 3 colas (Q2, Q1, Q0) con quantum de 10 ms cada una, verificados con el simulador `mlfq.py`.

### Ejemplo 1: Proceso CPU-bound

Un único proceso A (arrival = 0, run-time = 200 ms).

```
./mlfq.py -l 0,200,0 -q 10 -c
```

A entra en Q2 (Regla 3), agota su quantum de 10 ms y baja a Q1 (Regla 4a), agota nuevamente su quantum y baja a Q0, donde permanece ejecutándose (por ser la cola de menor prioridad) hasta completarse en t=200.

### Ejemplo 2: Llega un Proceso Corto

Se añade un proceso B corto: A (arrival=0, run-time=180 ms) y B (arrival=100, run-time=20 ms).

```
./mlfq.py -l 0,180,0:100,20,0 -q 10 -c
```

A desciende por las colas igual que en el Ejemplo 1, y hacia t=100 ya se encuentra ejecutándose en Q0. Al llegar B en t=100, este ingresa en Q2 (Regla 3) y expropia la CPU. B agota su primer quantum en Q2 y desciende a Q1 (Regla 4a), donde termina de ejecutarse y finaliza en t=120. El planificador retoma entonces a A, que continúa en Q0 hasta terminar en t=200.

### Ejemplo 3: Proceso I/O-bound

Se reemplaza B por un proceso interactivo: A (arrival=0, run-time=200) y B (arrival=50, CPU-burst=1 ms, I/O-burst=9 ms).

```
./mlfq.py -l 0,200,0:50,15,1 -q 10 -i 9 -c
```

B nunca agota su quantum, pues cede la CPU tras solo 1 ms para realizar una operación de I/O de 9 ms (Regla 4b). Por esta razón, B permanece siempre en Q2, la cola de mayor prioridad. Mientras B está bloqueado en I/O, el planificador ejecuta a A, que permanece degradado en Q0. El MLFQ logra así mantener al proceso interactivo con la prioridad más alta durante toda su ejecución.

### Ejemplo Resumen: Tres Procesos

Con un *workload* de tres procesos — A (*long-running*, CPU-bound), B (*short-running*) y C (interactivo) — se observa el comportamiento combinado: A desciende progresivamente hasta Q0 y permanece allí ejecutando en ráfagas de 10 ms; B, al llegar, interrumpe a A desde Q2, pasa brevemente por Q1 y termina; C, por ser interactivo, se mantiene siempre en Q2 con ráfagas cortas intercaladas de I/O.

## Problemas del MLFQ Clásico

El MLFQ clásico (Reglas 1 a 5, con 4a/4b) presenta dos problemas principales.

### Cambio de Comportamiento

Un proceso puede cambiar su comportamiento con el tiempo (por ejemplo, de CPU-bound a I/O-bound), y las reglas básicas no reaccionan explícitamente a ese cambio.

### Inanición (*Starvation*)

Si hay muchos procesos interactivos, los procesos CPU-bound pueden **nunca** recibir tiempo de CPU: el proceso en la cola más baja no se atiende mientras existan procesos en colas de mayor prioridad. En el ejemplo del manuscrito, un proceso A (CPU-bound) desciende a Q0 y queda indefinidamente relegado mientras procesos interactivos (B, C) ocupan Q2 de forma continua.

**Solución — Regla 5 (Priority Boost / *Aging*)**: después de un tiempo predeterminado **S** (S=50 ms en el ejemplo ilustrado), todos los procesos se mueven de vuelta a la cola de mayor prioridad, garantizando que ningún proceso quede bloqueado indefinidamente.

### Engañar al Planificador (*Gaming the Scheduler*)

Un proceso malicioso puede liberar la CPU justo antes de que expire su quantum para permanecer siempre en la cola de mayor prioridad y monopolizar el uso de la máquina. En el ejemplo del manuscrito, un proceso B ejecuta 9 ms de CPU (90% de un quantum de 10 ms) y libera la CPU con una operación de I/O de 1 ms, manteniéndose siempre en Q2 por la Regla 4b, mientras un proceso A normal queda relegado a Q0.

**Solución — Nueva Regla 4 (mejora del conteo / *better accounting*)**: reemplaza a las Reglas 4a y 4b. Lleva un conteo **acumulado** del tiempo de CPU consumido por un proceso en un nivel, sin importar cuántas veces haya liberado y retomado la CPU. Cuando ese acumulado alcanza el quantum de la cola, el proceso baja de prioridad — eliminando la posibilidad de manipular al planificador.

> [!Note]
> **Nueva Regla 4**: Cuando un trabajo consume su asignación de tiempo en un nivel, su prioridad es reducida (sin importar las veces que este retome el uso de la CPU).

## MLFQ Mejorado: Reglas Consolidadas

* **Regla 1**: Si prioridad(A) > prioridad(B), A se ejecuta.
* **Regla 2**: Si prioridad(A) = prioridad(B), RR para A y B.
* **Regla 3**: Cuando un trabajo llega al sistema, se ubica en la cola con la prioridad más alta.
* **Nueva Regla 4**: Cuando un trabajo consume su asignación de tiempo en un nivel, su prioridad es reducida (sin importar las veces que retome el uso de la CPU).
* **Regla 5**: Después de un tiempo S, mueva todos los trabajos al mayor nivel de prioridad.

## Ajustes Adicionales: Quantum Variable por Cola

Una mejora adicional consiste en asignar un quantum distinto según el nivel de prioridad: **menor nivel de prioridad → mayor tiempo de quantum**.

* **Alta prioridad**: quantum corto (~10 ms) — favorece el tiempo de respuesta de los procesos interactivos.
* **Baja prioridad**: quantum largo (~100 ms) — reduce el número de cambios de contexto para procesos CPU-bound, favoreciendo el turnaround time.

## Resumen y Conclusión

Con el MLFQ se cierra el estudio de la planificación de CPU del Módulo 1. Partiendo de un planificador que no sabe nada sobre los procesos, el MLFQ construye — únicamente a partir del comportamiento observado (uso de CPU vs. cesiones para I/O) — una aproximación a SJF/STCF para procesos cortos e interactivos, y a la equidad de Round Robin para los de larga duración, sin necesitar conocer el tiempo de ejecución por adelantado.

| Regla | Propósito |
| --- | --- |
| 1 y 2 | Ejecutar siempre la mayor prioridad; repartir con RR entre iguales. |
| 3 | Dar el beneficio de la duda a todo proceso nuevo (máxima prioridad). |
| Nueva Regla 4 | Degradar la prioridad según el tiempo de CPU realmente consumido, evitando el engaño al planificador. |
| 5 | Reforzar periódicamente la prioridad de todos los procesos, evitando la inanición. |

> [!tip]
> El MLFQ alcanza un equilibrio notable entre el tiempo de retorno (turnaround) y el tiempo de respuesta (response time), sin requerir conocimiento previo de la duración de los procesos.

---

> [!IMPORTANT]
> **Nota de Transparencia:** Este documento fue generado y adaptado mediante el uso de **IA Generativa**, a partir del manuscrito anotado de la clase y los resúmenes de las sesiones de Zoom del 25/08/2026 y 27/08/2026. El contenido ha sido supervisado, validado y refinado por intervención humana para garantizar su precisión técnica y coherencia pedagógica. No obstante, pueden haber errores.
