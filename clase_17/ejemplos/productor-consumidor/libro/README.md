# Guía práctica: Variables de Condición y el Problema Productor–Consumidor

> [!important]
> Estos ejemplos fueron tomados y adaptados (algunos de ellos) del libro de Operating Systems: Three Easy Pieces ([link](https://pages.cs.wisc.edu/~remzi/OSTEP/)). 


## 1. Introducción

Esta guía práctica tiene como propósito que el estudiante comprenda, experimente y analice el uso correcto de **variables de condición (Condition Variables, CV)** en la resolución del clásico problema del **Productor–Consumidor**.

El trabajo se desarrolla mediante cuatro implementaciones incrementales, cada una con errores o mejoras específicas que permiten ilustrar principios fundamentales de sincronización en sistemas operativos.

La práctica se basa en los conceptos del libro *Operating Systems: Three Easy Pieces (OSTEP)*, capítulo *Condition Variables*.

## 2. Conceptos esenciales de variables de condición

Antes de ejecutar los programas, es fundamental repasar tres conceptos críticos necesarios para la comprensión de los errores inducidos en las versiones preliminares:

1.  **Exclusión Mutua:** El acceso a las variables de estado compartido (como los índices del buffer) debe estar estrictamente protegido por un *mutex* (lock) durante toda la operación crítica. Las  operaciones clave son:
    - `pthread_mutex_lock(m)`
    - `pthread_mutex_unlock(m)`

2.  **Definición y Operaciones:** Una Variable de Condición actúa como una cola explícita donde los hilos pueden esperar (*wait*) a que ocurra un cambio de estado específico. Las operaciones principales son `wait()` (que libera el lock y duerme el hilo) y `signal()` (que despierta a un hilo en espera).
    
    * **`pthread_cond_wait(cond, mutex)`**: 
      - Libera el mutex **atómicamente**.
      - Bloquea el hilo que invoca la operación.
      - Al despertar, readquiere el mutex antes de retornar.
    * **`pthread_cond_signal(cond)`**:
      - Despierta un único hilo que esté esperando en la variable de condición.
      - Si no hay hilos esperando, no tiene efecto.

3.  **La Regla de Oro (Always Use While):** Al despertar de un `wait()`, no se garantiza que la condición por la cual el hilo se durmió siga siendo verdadera (semántica Mesa/Hansen). Por ello, **siempre** se debe verificar la condición de estado dentro de un bucle `while` y nunca mediante un simple `if`.

    ```c
    pthread_mutex_lock(&m);
    while (condicion_no_cumplida)
       pthread_cond_wait(&cv, &m);
    /* sección donde la condición se cumple */
    pthread_mutex_unlock(&m);
    ``` 

* 4. **Estado + CV**: La CV **no almacena estado**. El estado debe mantenerse mediante variables como:
   - `count`
   - `numfull`
   - punteros del buffer (`fillptr`, `useptr`)  

## 3. Archivos de la guía

A continuación se detalla el funcionamiento de cada versión del código fuente provisto. Cada archivo representa una etapa incremental en la solución del problema.

| Archivo      | Descripción breve |
|--------------|-------------------|
| `main_v1.c` | Una sola CV, uso de `while` pero señalización incorrecta. |
| `main_v2.c` | Dos CV (`empty`, `fill`) pero uso incorrecto de `if`. |
| `main_v3.c` | Dos CV correctamente usadas con `while`. Solución estándar. |
| `main_v4.c` | Mutex liberado antes de operar en el buffer. Versión didáctica. |



## 4. Ejecución de los programas

> [!warning]
Todos los códigos dependen de la librería auxiliar `mythreads.h` incluida en el directorio. 

### 4.1. Compilación

```bash
gcc -I. main_vX.c -Wall -lpthread -o vX.out
```

Reemplace `X` por la versión 1–4.

> [!note]
> Se dispone de un Makefile para no tener que ejecutar la compilación individual para cada archivo fuente.

### 4.2. Ejecución

```bash
./vX.out <buffersize> <loops> <consumers>
```

Ejemplo:

```bash
./v1.out 1 20 3
```

Parámetros:
- `buffersize`: tamaño del buffer.
- `loops`: cantidad de ítems producidos.
- `consumers`: número de consumidores.

## 5. Análisis Caso por Caso

### 5.1. Versión 1 — main_v1.c: El Error de la Variable Única

* **Descripción**: Implementa una única variable de condición (cond) para manejar tanto la señalización de "buffer lleno" como de "buffer vacío". Utiliza un bucle while para la verificación.
* **Problema**: Sufre del problema de despertar equivocado. Un consumidor puede despertar a otro consumidor en lugar de a un productor (o viceversa). Esto lleva a un estado donde todos los hilos terminan durmiendo, causando un interbloqueo (deadlock).

### 5.2. Versión 2 — main_v2.c

* **Descripción**: Introduce dos variables de condición separadas (empty y fill) para diferenciar a productores de consumidores. Sin embargo, utiliza la sentencia if para verificar el estado del buffer.
* **Problema**: Vulnerable a condiciones de carrera. Si un hilo despierta pero otro hilo se "cuela" y modifica el buffer antes de que el primero retome el lock, el primer hilo actuará sobre un estado inválido (ej. intentar consumir de un buffer vacío), provocando errores de ejecución o corrupción de datos.

### 5.3. Versión 3 — main_v3.c (Solución correcta)

* **Descripción**: Combina el uso de dos variables de condición (empty y fill) con la verificación de estado mediante bucles while.

* **Estado**: Comportamiento *esperado*. Esta versión maneja adecuadamente los despertares espurios y asegura que los hilos solo procedan cuando el estado es válido, respetando la exclusión mutua. Representa la implementación estándar segura.

### 5.4. Versión 4 — main_v4.c 

**Descripción**: Esta versión libera el mutex antes de llamar a `do_fill()`/`do_get()`.

* **Comportamiento esperado:**  
  - Mayor concurrencia.
  - **Potencialmente riesgosa** si se agregan operaciones adicionales.
  - Es una versión **didáctica**, aun tiene problemas.

## 6. Tabla comparativa

| Versión | # CV | Uso de `while` | Corrección | Problema principal | Caso en que funciona |
|--------|------|-----------------|------------|--------------------|----------------------|
| **v1** | 1    | Sí              |  Incorrecta | Consumidores despiertan consumidores; deadlock posible | 1 productor + 1 consumidor |
| **v2** | 2    |  No           |  Incorrecta | Wakeups espurios; revalidación insuficiente | Casos simples |
| **v3** | 2    |  Sí          |  Correcta | Ninguno | Cualquier cantidad de hilos |
| **v4** | 2    |  Sí          |  Parcial | Riesgo al operar sin mutex | Uso educativo |

## 7. Resultados de Aprendizaje Esperados

Al finalizar la ejecución y análisis de estos códigos, se espera que el estudiante sea capaz de adquirir las siguientes habilidades claves.

### 7.1. Conceptuales
- Comprender el funcionamiento de variables de condición en POSIX.
- Diferenciar entre `if` y `while` en sincronización.
- Identificar errores como deadlocks, wakeups espurios y despertares incorrectos.
- Reconocer que las CV no contienen estado; solo coordinan hilos.

### 7.2. Procedimentales
- Compilar programas concurrentes con `pthread`.
- Observar empíricamente fallos de sincronización.
- Comparar implementaciones incrementales y sus consecuencias.

### 7.3. Actitudinales
- Adoptar buenas prácticas en programación concurrente.
- Desarrollar criterio técnico para identificar patrones correctos de sincronización.
- Reconocer la importancia de diseñar correctamente sistemas concurrentes.

## 8. Conclusión

Las cuatro implementaciones permiten explorar la evolución del problema productor–consumidor desde soluciones incorrectas hasta la versión correcta y robusta. Esta guía prepara al estudiante para enfrentar problemas reales de sincronización en sistemas operativos modernos.

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. 