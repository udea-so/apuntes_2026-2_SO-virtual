# Ejemplos de Variables de Condición: El Problema del Productor-Consumidor

Este repositorio contiene una serie de ejemplos progresivos que implementan el problema clásico del **Productor-Consumidor** (o Buffer Acotado). El objetivo es ilustrar los problemas comunes de concurrencia y cómo solucionarlos usando primitivas de `pthreads`, específicamente `mutex` y **variables de condición**.

El problema base consiste en:
* **Productor:** Genera ítems (datos) y los pone en un buffer compartido.
* **Consumidor:** Retira ítems del buffer y los consume.

La sincronización es esencial para evitar que el productor escriba en un buffer lleno o que el consumidor lea de un buffer vacío.

## The Producer/Consumer (Bounded Buffer) Problem

### Funciones `put` y `get` (Base)

```c
int buffer;
int count = 0; // initially, empty

void put(int value) {
  assert(count == 0);
  count = 1;
  buffer = value;
}

int get() {
  assert(count == 1);
  count = 0;
  return buffer;
}
````

El `assert` es una macro utilizada para verificar que una condición se cumpla. La macro evalua la condición y si esta es falsa `assert` imprimirá un mensaje de error y terminará el programa.

El código del productor y el consumidor (sin sincronización) se muestra a continuación:

**Productor**

```c
void *producer(void *arg) {
  int i;
  int loops = (int) arg;
  for (i = 0; i < loops; i++) {
    put(i);
  }
}
```

**Consumidor**

```c
void *consumer(void *arg) {
  while (1) {
    int tmp = get();
    printf("%d\n", tmp);
  }
}
```

## Progresión de Ejemplos (Buffer de Tamaño 1)

Empezamos con un buffer que solo puede almacenar un ítem a la vez (`count` solo puede ser 0 o 1).

### Caso 0 - No se maneja primitivas de concurrencia

  * **codigo**: [main\_singleBuffer-v0.c](https://www.google.com/search?q=main_singleBuffer-v0.c).
  * **Descripción:** Esta es la implementación "ingenua" o base. Utiliza las funciones `put()` y `get()` sin ningún tipo de *mutex* o variable de condición.
  * **Análisis:** Este código fallará catastróficamente. Los `assert(count == 0)` y `assert(count == 1)` fallarán casi de inmediato debido a condiciones de carrera, donde el productor y el consumidor (o múltiples consumidores) intentan modificar la variable `count` y el `buffer` al mismo tiempo.

-----

### Caso 1 - Unica CV y sentencia if - Broken solution

  * **codigo**: [main\_singleBuffer-v1.c](https://www.google.com/search?q=main_singleBuffer-v1.c)
  * **Descripción:** Primera solución usando un *mutex* para proteger la región crítica y **una sola variable de condición** (`cond`). Crucialmente, usa una sentencia `if` para chequear si el buffer está lleno o vacío.
  * **Análisis (Incorrecto):** Esta solución es errónea y demuestra el **Problema 1** identificado en las diapositivas: una **condición de carrera**.
      * **Escenario de Falla:** Funciona con 1 productor y 1 consumidor, pero falla con múltiples consumidores.
      * **Por qué falla (El "Sneak In"):** Un consumidor `C1` puede encontrar el buffer vacío (`count == 0`) e irse a dormir (`pthread_cond_wait`). El productor produce un ítem (`put(i)`) y despierta a `C1` (`pthread_cond_signal`). PERO, antes de que `C1` pueda readquirir el mutex y ejecutarse, otro consumidor `C2` "se cuela" (`sneaks in`), toma el mutex, consume el ítem (`get()`) y lo libera. Cuando `C1` finalmente se ejecuta (retornando de `pthread_cond_wait`), *no vuelve a chequear la condición* (porque era un `if`) y ejecuta `get()` sobre un buffer que ya está vacío. Esto dispara el `assert(count == 1)`.

**Líneas clave:**

```c
// En Productor:
	if (count == 1)                 	   // p2
  	  pthread_cond_wait(&cond, &mutex);    // p3
	put(i);                         	   // p4
	pthread_cond_signal(&cond);     	   // p5

// En Consumidor:
	if (count == 0)                   	  // c2
  	  pthread_cond_wait(&cond, &mutex);    // c3
	tmp = get();                      	  // c4
	pthread_cond_signal(&cond);       	  // c5
```

**Desafío: Cómo Ver el Fallo**

1.  **Ejecuta** el código con 2 o más consumidores: `./main_singleBuffer-v1.out 1000 2`
2.  **Observa** cómo el programa falla con el `assert(count == 1)`.
3.  **Instrumenta el código:** Para ver la traza de la diapositiva, añade `printf`s antes y después de `pthread_cond_wait` en el consumidor.
   
    ```c
    // En consumer() de v1:
    // (Asegúrate de pasar un ID al hilo consumidor para ver esto claramente)
    if (count == 0) {
      printf("Consumidor %d: Buffer vacío, voy a dormir (if).\n", id);
      pthread_cond_wait(&cond, &mutex);
      printf("Consumidor %d: ¡Desperté! Count es %d.\n", id, count);
    }
    tmp = get(); // <-- Aquí falla
    ```
    *Verás cómo un consumidor "¡Despierta\!" pero, antes de que pueda hacer `get()`, otro consumidor se cuela y roba el dato.*

-----

### Caso 2 - Unica CV y sentencia while

  * **Codigo**: [main\_singleBuffer-v2.c](https://www.google.com/search?q=main_singleBuffer-v2.c)
  * **Descripción:** Esta versión corrige el **Problema 1** reemplazando el `if` por un `while`. Esta es la "regla de oro" de las variables de condición.
  * **Análisis (Incorrecto):** Esta solución arregla la condición de carrera (el consumidor despertado siempre "re-chequea" el estado), pero introduce el **Problema 2**: un **Deadlock**.
      * **Escenario de Falla:** El programa se "congela" o bloquea.
      * **Por qué falla (Señalización no específica):** Se usa una sola CV (`cond`) tanto para productores como para consumidores. El *deadlock* ocurre cuando, por ejemplo, un consumidor (`C1`) consume el último ítem y envía una señal (`pthread_cond_signal`). Esta señal estaba destinada al productor (para que sepa que hay espacio), pero es recibida por otro consumidor (`C2`) que estaba dormido. `C2` se despierta, ve que el buffer *sigue vacío* (gracias al `while`), y se vuelve a dormir. Mientras tanto, el productor sigue dormido (porque nunca recibió la señal) y `C1` se va a dormir. Eventualmente, todos los hilos quedan dormidos esperando una señal que nunca llegará.

**Líneas clave:**

```c
// En Productor:
	while (count == 1)              	   // p2
  	  pthread_cond_wait(&cond, &mutex);    // p3
	put(i);                         	   // p4
	pthread_cond_signal(&cond);     	   // p5

// En Consumidor:
	while (count == 0)                    // c2
  	  pthread_cond_wait(&cond, &mutex);   // c3
	tmp = get();                      	  // c4
	pthread_cond_signal(&cond);       	  // c5
```

**Desafío: Cómo Ver el Fallo**

1.  **Ejecuta** el código: `./main_singleBuffer-v2.out 1000 2`
2.  **Observa** cómo el programa se "congela" (deadlock) después de un tiempo.
3.  **Instrumenta el código:** Para ver la traza del deadlock de las diapositivas, añade `printf`s.
    ```c
    // En consumer() de v2:
    while (count == 0) {
      printf("Consumidor %d: Buffer vacío, voy a dormir (while).\n", id);
      pthread_cond_wait(&cond, &mutex);
      printf("Consumidor %d: Desperté, re-chequeando...\n", id);
    }
    tmp = get();
    printf("Consumidor %d: Consumí %d. Señalizando.\n", id, tmp);
    pthread_cond_signal(&cond);
    ```
    *Verás la traza fatal: C1 consume, señaliza y despierta a C2 (error). C2 despierta, ve el buffer vacío y vuelve a dormir. El productor sigue dormido. Todos duermen.*

**Conclusiones de las diapositivas**:

  * Señalizar es necesario.
  * La señalizacion llevada a cabo debe ser *mas especifica*.

-----

### Caso 3 - Se usan dos variables de condicion y un While

  * **Código**: [main\_singleBuffer-v3.c](https://www.google.com/search?q=main_singleBuffer-v3.c)
  * **Descripción:** La solución **correcta** para un buffer de un solo ítem. Implementa la lección del Problema 2: la señalización debe ser específica.
  * **Análisis (Correcto):** Se usan **dos variables de condición**:
      * `empty`: Los productores esperan en esta CV si el buffer está lleno (`count == 1`).
      * `fill`: Los consumidores esperan en esta CV si el buffer está vacío (`count == 0`).
      * **Flujo:** Cuando un productor pone un ítem, señaliza `fill` (despierta a un consumidor). Cuando un consumidor toma un ítem, señaliza `empty` (despierta a un productor). Esto elimina el *deadlock* porque la señalización es específica y dirigida.

**Líneas clave:**

```c
// Inicializacion
cond_t empty, fill;

// En Productor:
	while (count == 1)
  	  pthread_cond_wait(&empty, &mutex);
	put(i);
	pthread_cond_signal(&fill);

// En Consumidor:
	while (count == 0)
  	  pthread_cond_wait(&fill, &mutex);
	int tmp = get();
	pthread_cond_signal(&empty);
```

-----

## Solución Final (Buffer Múltiple Acotado)

Las soluciones anteriores se expanden para usar un buffer de tamaño `max` (un buffer circular).

### Caso 4 - Buffer Múltiple (Bloqueo al Final)

  * **Codigo**: [main\_buffer-v3.c](https://www.google.com/search?q=main_buffer-v3.c)
  * **Descripción:** Implementa la solución final y correcta del buffer acotado usando un buffer circular (con `fill_ptr`, `use_ptr`) y las dos variables de condición (`empty` y `fill`).
  * **Análisis:** La lógica de sincronización es sólida:
      * Productor duerme en `empty` si `count == max`.
      * Consumidor duerme en `fill` si `count == 0`.
      * **Problema:** Este programa funciona, pero **se bloquea al final**. El productor termina (produce sus `loops`), pero los hilos consumidores quedan atrapados en un `while(1)`, durmiendo eternamente en `pthread_cond_wait(&fill, ...)` porque no saben que el productor ha terminado y no llegarán más ítems.

**Funciones `put` y `get` (Buffer Múltiple)**

```c
int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;

void put(int value) {
  buffer[fill_ptr] = value;
  fill_ptr = (fill_ptr + 1) % MAX;
  count++;
}

int get() {
  int tmp = buffer[use_ptr];
  use_ptr = (use_ptr + 1) % MAX;
  count--;
  return tmp;
}
```

**Productor (Buffer Múltiple)**

```c
void *producer(void *arg) {
  int i;
  for (i = 0; i < loops; i++) {
	pthread_mutex_lock(&mutex);      	     // p1
	while (count == MAX)             	     // p2
  	  pthread_cond_wait(&empty, &mutex);     // p3
	put(i);                          	     // p4
	pthread_cond_signal(&fill);      	     // p5
	pthread_mutex_unlock(&mutex);    	     // p6
  }
}
```

**Consumidor (Buffer Múltiple)**

```c
void *consumer(void *arg) {
  while(1) {
	pthread_mutex_lock(&mutex);     	 // c1
	while (count == 0)              	 // c2
  	  pthread_cond_wait(&fill, &mutex);  // c3
	int tmp = get();                	 // c4
	pthread_cond_signal(&empty);    	 // c5
	pthread_mutex_unlock(&mutex);   	 // c6
	printf("%d\n", tmp);
  }
}
```

-----

### Caso 5 - Solución Final (con Terminación)

  * **Código**: [main\_buffer-v4.c](https://www.google.com/search?q=main_buffer-v4.c)
  * **Descripción:** La solución final **completa y robusta**. Es idéntica a `v3`, pero añade una **lógica de terminación**.
  * **Análisis (Correcto y Finaliza):**
      * **Productor:** Después de producir sus `loops` ítems, el productor entra en un nuevo bucle. En este bucle, coloca un ítem especial (una **marca de terminación**, en este caso `-1`) en el buffer, *una vez por cada hilo consumidor*.
      * **Consumidor:** El `while(1)` se cambia a `while (tmp != -1)`. Cuando un consumidor obtiene el `-1`, sale del bucle y el hilo termina.
      * **Por qué un `-1` por consumidor?** Es vital. Si el productor solo enviara un `-1`, solo un consumidor lo recibiría y terminaría. Los otros consumidores se quedarían bloqueados esperando un ítem que nunca llegará. Al enviar `consumers` número de marcas `-1`, el productor se asegura de que *cada* hilo consumidor reciba la señal de terminación.

**Lógica de Terminación (En el Productor de `v4`)**

```c
  // ... Bucle principal de producción termina ...

  // end case
  for (i = 0; i < consumers; i++) {
    Mutex_lock(&mutex);
    while (count == max) 
      Cond_wait(&empty, &mutex);
	  put(-1);
	  Cond_signal(&fill);
	  Mutex_unlock(&mutex);
  }
```

**Lógica de Terminación (En el Consumidor de `v4`)**

```c
void *consumer(void *arg) {
  int tmp = 0;
  while (tmp != -1) { // <-- Condición de fin
    Pthread_mutex_lock(&mutex);
    while(count == 0)
      pthread_cond_wait(&fill, &mutex);
    tmp = get();
    pthread_cond_signal(&empty);
    Pthread_mutex_unlock(&mutex);
    if (verbose) printf("%d\n", tmp);
  }
  return NULL;
}
```

-----

## Compilación y Ejecución

Los archivos `mythreads.h` (que provee *wrappers* de `assert` para las funciones Pthreads) y `Makefile` están incluidos.

Para compilar todos los ejemplos:

```bash
make
```

Para ejecutar la solución final (Buffer de 10, 1000 ítems, 2 consumidores):

```bash
./main_buffer-v4.out 10 1000 2
```

-----

## Resultados de Aprendizaje Esperados

Al finalizar el análisis de estos ejemplos en conjunto con las diapositivas, se espera que el estudiante sea capaz de:

  * **Comprender el Problema de la Región Crítica**: Identificar por qué el acceso concurrente a datos compartidos (como un buffer) crea condiciones de carrera y cómo usar mutex para garantizar la exclusión mutua y proteger esa región crítica.
  * **Dominar la Sincronización de Condición**: Entender por qué los mutex por sí solos son insuficientes (llevan a espera activa o *busy-waiting*) y cómo las variables de condición (wait y signal) proporcionan un mecanismo eficiente para que los hilos duerman y despierten.
  * **Implementar Esperas (Wait) Robustas**: Interiorizar la "regla de oro": **siempre usar un bucle `while`** para re-chequear la condición después de despertar de un `pthread_cond_wait`. Esto permitirá prevenir condiciones de carrera sutiles que ocurren cuando un hilo se "cuela" antes de que el hilo despertado se ejecute (el fallo del `if` en `v1`).
  * **Implementar Señalización (Signal) Específica**: Aprender a diagnosticar y prevenir deadlocks causados por señalización ambigua (el fallo de `v2`). Esto se logra usando múltiples variables de condición (como `empty` y `fill`) para dirigir las señales inequívocamente al tipo de hilo correcto (productor o consumidor).

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. 

> [!warning]
> **Aclaración**: Como todo código (especialmente los ejemplos didácticos diseñados para fallar), el contenido debe ser revisado críticamente y puede contener errores.
