# Implementación del planificador MLFQ

**Objetivo**: Implementar el algoritmo de planificación **MLFQ** con al menos **3 colas**, cada una con un **quantum de tiempo diferente**. Los procesos deben moverse entre las colas según su comportamiento y una estimación de su duración en CPU.

El algoritmo de planificación **Multi-Level Feedback Queue (MLFQ)** es un algoritmo complejo de planificación de CPU diseñado para optimizar el **turnaround time**, el **response time** y la **utilización de la CPU**. Utiliza múltiples colas con diferentes niveles de prioridad, ajustando la prioridad de los procesos según su comportamiento y sus requerimientos.

## Cómo funciona MLFQ

1. **Múltiples colas**: MLFQ utiliza varias colas con diferentes niveles de prioridad. Una cola de mayor prioridad tiene un **quantum de tiempo más corto**, mientras que una cola de menor prioridad tiene un **quantum más largo**.
2. **Ajuste de prioridades**: Los procesos comienzan en la cola de **mayor prioridad**. Si un proceso utiliza todo su quantum de tiempo sin terminar, se mueve a una cola de **menor prioridad**. Si un proceso libera la CPU antes de que termine su quantum, permanece en la misma cola.
3. **Boosting**: Para evitar **inanición (starvation)**, existe un mecanismo que periódicamente aumenta la prioridad de los procesos que se encuentran en colas de menor prioridad.

## Reglas de MLFQ

1. Si **Priority(A) > Priority(B)**, entonces se ejecuta **A** (B no se ejecuta).
2. Si **Priority(A) = Priority(B)**, A y B se ejecutan en **Round Robin**, usando el quantum de tiempo de la cola correspondiente.
3. Cuando un trabajo entra al sistema, se ubica en la **máxima prioridad** (la cola superior).
4. Una vez que un trabajo utiliza todo su tiempo asignado en un nivel dado (sin importar cuántas veces haya recibido CPU), **su prioridad se reduce** (es decir, se mueve a la siguiente cola inferior).
5. Después de un periodo de tiempo $S$, **todos los trabajos del sistema se mueven nuevamente a la cola de mayor prioridad**.

---

## Implementación del planificador MLFQ en C

### Paso 1: Incluir las librerías necesarias

Antes de comenzar a implementar el algoritmo MLFQ, necesitamos configurar nuestro entorno de programación incluyendo las librerías necesarias.

```c
#include <stdio.h>
#include <stdlib.h>
```

### Paso 2: Definir las estructuras de Proceso y Cola

Necesitamos definir dos estructuras: `Process` y `Queue`.

* **`Process`**: representa un proceso con atributos como `id`, `duration`, `remaining_time`, `waiting_time` y `turnaround_time`.
* **`Queue`**: representa una cola que contiene procesos. Tiene atributos como `processes` (un arreglo de punteros a `Process`), `front`, `rear` y `time_quantum`.

```c
typedef struct {
    int id;
    int duration;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
} Process;

typedef struct {
    Process *processes[100];
    int front, rear;
    int time_quantum;
} Queue;
```

### Paso 3: Inicializar la cola

Necesitamos una función que inicialice nuestra cola. Esta función establece los punteros `front` y `rear` en **-1** (lo cual indica una cola vacía) y define el **quantum de tiempo** asociado a la cola.

```c
void initializeQueue(Queue *q, int time_quantum) {
    q->front = q->rear = -1;
    q->time_quantum = time_quantum;
}
```

### Paso 4: Implementar las operaciones de la cola

También necesitamos funciones para **agregar (`enqueue`)** y **remover (`dequeue`)** procesos de la cola.

* **`enqueue`**: agrega un proceso al final de la cola.
* **`dequeue`**: elimina un proceso del frente de la cola y lo retorna.

```c
void enqueue(Queue *q, Process *p) {
    if(q->rear == 99) {
        printf("Queue is full!\n");
        return;
    }
    q->processes[++q->rear] = p;
    if(q->front == -1) {
        q->front = 0;
    }
    printf("Process %d enqueued in queue with time quantum %d\n", p->id, q->time_quantum);
}

Process* dequeue(Queue *q) {
    if(q->front == -1) {
        return NULL;
    }
    Process *p = q->processes[q->front];
    if(q->front == q->rear) {
        q->front = q->rear = -1;
    } else {
        q->front++;
    }
    printf("Process %d dequeued from queue with time quantum %d\n", p->id, q->time_quantum);
    return p;
}
```

### Paso 5: Implementar la lógica de planificación MLFQ

Esta es la parte central del algoritmo de planificación MLFQ. Aquí se extraen procesos de la cola de **mayor prioridad** y se les asigna tiempo de CPU de acuerdo con el **quantum de tiempo** de su cola. Si un proceso no termina dentro de su quantum, se mueve a una cola de **menor prioridad**.

**En esta implementación**:

* Primero intentamos obtener un proceso de la cola de **alta prioridad**.
* Si encontramos un proceso, verificamos si puede terminar dentro del quantum de esa cola.
  * Si puede terminar, actualizamos el tiempo total, el tiempo de espera y el turnaround time.
  * Si no puede terminar, reducimos su tiempo restante y lo movemos a la cola de **prioridad media**.
* Si no hay procesos en la cola de alta prioridad, revisamos la cola de **prioridad media** y repetimos la misma lógica.
* Si tampoco hay procesos en esa cola, revisamos la cola de **baja prioridad**.
  * En esta cola, si un proceso no termina dentro de su quantum, recibe otra ronda en la misma cola.
* El ciclo continúa hasta que **todas las colas estén vacías**.

```c
void mlfq_scheduling(Queue *high_priority_q, Queue *medium_priority_q, Queue *low_priority_q, int n) {
    int total_time = 0;
    while(1) {
        Process *p = dequeue(high_priority_q);
        if(p != NULL) {
            printf("Process %d is running in high priority queue\n", p->id);
            if(p->remaining_time <= high_priority_q->time_quantum) {
                total_time += p->remaining_time;
                p->remaining_time = 0;
                p->waiting_time = total_time - p->duration;
                p->turnaround_time = total_time;
                printf("Process %d finished execution\n", p->id);
            } else {
                p->remaining_time -= high_priority_q->time_quantum;
                total_time += high_priority_q->time_quantum;
                enqueue(medium_priority_q, p);
            }
        } else {
            p = dequeue(medium_priority_q);
            if(p != NULL) {
                printf("Process %d is running in medium priority queue\n", p->id);
                if(p->remaining_time <= medium_priority_q->time_quantum) {
                    total_time += p->remaining_time;
                    p->remaining_time = 0;
                    p->waiting_time = total_time - p->duration;
                    p->turnaround_time = total_time;
                    printf("Process %d finished execution\n", p->id);
                } else {
                    p->remaining_time -= medium_priority_q->time_quantum;
                    total_time += medium_priority_q->time_quantum;
                    enqueue(low_priority_q, p);
                }
            } else {
                p = dequeue(low_priority_q);
                if(p != NULL) {
                    printf("Process %d is running in low priority queue\n", p->id);
                    if(p->remaining_time <= low_priority_q->time_quantum) {
                        total_time += p->remaining_time;
                        p->remaining_time = 0;
                        p->waiting_time = total_time - p->duration;
                        p->turnaround_time = total_time;
                        printf("Process %d finished execution\n", p->id);
                    } else {
                        p->remaining_time -= low_priority_q->time_quantum;
                        total_time += low_priority_q->time_quantum;
                        enqueue(low_priority_q, p);
                    }
                } else {
                    break;
                }
            }
        }
    }
}
```

### Paso 6: Implementar la función `main()`

La función `main()` es el **punto de entrada del programa**, donde comienza la ejecución. En el contexto del algoritmo MLFQ, esta función es responsable de:

* inicializar las colas
* recibir la entrada del usuario
* ejecutar el planificador
* mostrar los resultados

Veamos cada parte.

#### Paso 6.1: Inicializar las colas

Aquí inicializamos tres colas: **alta**, **media** y **baja prioridad**, cada una con su respectivo quantum de tiempo. El quantum determina cuánto tiempo de CPU recibe un proceso antes de ser movido a una cola inferior o finalizar su ejecución.

#### Paso 6.2: Solicitar al usuario el número de procesos

Pedimos al usuario que ingrese cuántos procesos serán planificados.

#### Paso 6.3: Reservar memoria para los procesos y capturar sus datos

Reservamos memoria para almacenar la información de los procesos y pedimos al usuario ingresar la duración de cada uno. Inicialmente, todos los procesos se agregan a la **cola de mayor prioridad**.

#### Paso 6.4: Ejecutar el planificador MLFQ

Invocamos la función `mlfq_scheduling()` para realizar la planificación usando el algoritmo MLFQ.

#### Paso 6.5: Mostrar los resultados

Una vez finalizada la planificación, mostramos los resultados: **ID del proceso, duración, tiempo de espera y turnaround time**.

#### Paso 6.6: Liberar la memoria asignada

Finalmente, liberamos la memoria utilizada para almacenar los procesos.

```c
int main() {
    // Step 6.1: Initialize the Queues
    Queue high_priority_q, medium_priority_q, low_priority_q;
    initializeQueue(&high_priority_q, 2);  // High priority queue with time quantum of 2
    initializeQueue(&medium_priority_q, 4); // Medium priority queue with time quantum of 4
    initializeQueue(&low_priority_q, 8);    // Low priority queue with time quantum of 8

    // Step 6.2: Take User Input for Number of Processes
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Step 6.3: Allocate Memory for Processes and Take Their Input
    Process *processes = (Process *)malloc(n * sizeof(Process));
    for(int i = 0; i < n; i++) {
        printf("Enter duration for process %d: ", i+1);
        scanf("%d", &processes[i].duration);
        processes[i].id = i+1;
        processes[i].remaining_time = processes[i].duration;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        enqueue(&high_priority_q, &processes[i]);  // Enqueue all processes to high priority queue initially
    }

    // Step 6.4: Invoke the MLFQ Scheduling Function
    mlfq_scheduling(&high_priority_q, &medium_priority_q, &low_priority_q, n);

    // Step 6.5: Display the Results
    printf("Process\tDuration\tWaiting Time\tTurnaround Time\n");
    for(int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].duration, processes[i].waiting_time, processes[i].turnaround_time);
    }

    // Step 6.6: Free the Allocated Memory
    free(processes);
    return 0;
}
```

---

## Tarea

Complile y ejecute el planificador MLFQ ([mlfq.c](mlfq.c)).

### Compilar

```
gcc mlfq_lab.c -o mlfq_lab
```

### Ejecutar

```
./mlfq_lab
```

### Probar

Pruebe con diferentes **cantidades de procesos** y **duraciones de ejecución**. Analice cómo cambian el **tiempo de espera** y el **turnaround time** según la carga de trabajo.
