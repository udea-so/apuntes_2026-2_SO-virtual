# Ejemplos de variables de condicion

## Parte 1 - Sobre la sincronización

### Problema

Dado el siguiente fragmento de código con hilos:

```c
void *child(void *arg) {
  printf("child\n");
  // XXX how to indicate we are done?
  return NULL;
}

int main(int argc, char *argv[]) {
  printf("parent: begin\n");
  pthread_t c;
  Pthread_create(&c, NULL, child, NULL); //create child
  // XXX how to wait for child?
  printf("parent: end\n");
  return 0;
}
```

¿Como hacer que la salida siempre sea la misma y de la forma?

```
parent: begin
child
parent: end
```

### Soluciones

#### Solucion 1 - Ineficiente

Esta solución ([ejemplo1.c](ejemplo1.c)) sirve pero el hilo padre desperdicia de CPU debido debido al spin-loop.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
gcc -Wall ejemplo1.c -lpthread -o ejemplo1
*/

volatile int done = 0;

void *child(void *arg) {
  printf("child\n");
  done = 1;
  return NULL;
}

int main(int argc, char *argv[]) {
  printf("parent: begin\n");
  pthread_t c;
  pthread_create(&c, NULL, child, NULL); // create child
  while (done == 0); // spin
  printf("parent: end\n");
  return 0;
}
```

#### Solucion 2 - Solución empleando variables de condicion

Esta es una solución mejor, pues el hilo padre espera hasta que se de una *condición* antes de continuar. Para esto se hacen uso de Variables de condicion (CV) las cuales permiten que se realicen ciertas acciones o bloques de código si se cumplen determinadas condiciones.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mythreads.h"

/*
gcc -Wall -I. ejemplo2.c -lpthread -o ejemplo2
*/

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

// Prototipos de las funciones
void thr_exit(void);
void *child(void *);
void thr_join(void);


// Funcion main

int main(int argc, char *argv[]) {
  printf("parent: begin\n");
  pthread_t p;
  Pthread_create(&p, NULL, child, NULL);
  thr_join();
  printf("parent: end\n");
  return 0;
}

// Definiciones de las funciones

void thr_exit(void) {
  Pthread_mutex_lock(&m);
  done = 1;
  Pthread_cond_signal(&c);
  Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
  printf("child\n");
  thr_exit();
  return NULL;
}

void thr_join(void) {
  Pthread_mutex_lock(&m);
  while (done == 0) {
	  Pthread_cond_wait(&c, &m);
  }
  Pthread_mutex_unlock(&m);
}
```

En el ejemplo anterior se emplean funciones claves `wait` y `signal`

## Parte 2 - Sobre las funciones `wait` y `signal`

En la biblioteca de hilos POSIX (Pthreads) se definen las funciones `pthread_cond_wait` y `pthread_cond_signal` para el manejo de variables de condición (CV). Estas son importantes pues permiten que varios hilos se puedan coordinar entre sí para acceder a recursos compartidos o ejecutar ciertas tareas en un orden específico.


### Funcion `pthread_cond_wait`

Sirve para suspender temporalmente la ejecución de un hilo hasta que se cumpla una condición específica.

> #### Sintaxis
> 
> ```c
> int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
> ```
> **Donde**:
> * **`cond`**: puntero a una variable de condición (`pthread_cond_t`) que se espera que cambie a un estado que permita continuar el hilo.
> * **`mutex`**: puntero a un mutex (`pthread_mutex_t`) que se desbloquea temporalmente cuando el hilo entra en espera y se vuelve a bloquear automáticamente cuando el hilo se despierta.

### Funcion `pthread_cond_signal`

Esta función se usa para despertar o notificar a uno de los hilos que están en espera en una variable de condición.

> #### Sintaxis
> 
> ```c
> int pthread_cond_signal(pthread_cond_t *cond)
> ```
>
> **Donde**:
> * **`cond`**: puntero a la variable de condición (`pthread_cond_t`) que se quiere señalizar para que despierte a un hilo en espera.

## Parte 3 - Implementación de funciones de sincronización

**Función `thr_exit()`**

```c
void thr_exit() {
  Pthread_mutex_lock(&m);
  done = 1;
  Pthread_cond_signal(&c);
  Pthread_mutex_unlock(&m);
}
```

**Función `thr_join()`**

```c
void thr_join() {
  Pthread_mutex_lock(&m);
  while (done == 0) 
	Pthread_cond_wait(&c, &m);
  Pthread_mutex_unlock(&m);
}
```

> * ¿Como hacer una reimplementación mas eficiente en la cual se logre que un hilo "padre" espere (haga join) a que un hilo "hijo" termine.
> * ¿Por qué es importante cada una de las instrucciones que se encuentran en `thr_join` y `thr_exit`?


### Implementación 1 - Implementación ROTA sin estado

**Codigo**: [implementacion1.c](implementacion1.c)

**Función `thr_exit()`**

```c
void thr_exit() {
  pthread_mutex_lock(&m);
  pthread_cond_signal(&c);
  pthread_mutex_unlock(&m);
}

```

**Función `thr_join()`**

```c
void thr_join() {
  pthread_mutex_lock(&m);
  pthread_cond_wait(&c, &m);
  pthread_mutex_unlock(&m);
}
```


Esta implementación **está rota** debido a aque no se usa una variable de estado (`done`). El fallo se debe a la perdida del **signal** (**Lost Wakeup**) lo cual genera un deadlock.

### Implementación 2 - La implementación ROTA con Race Condition

**Codigo**: [implementacion2.c](implementacion2.c)

**Función `thr_exit()`**

```c
void thr_exit() {
  done = 1;
  pthread_cond_signal(&c);
}
```

**Función `thr_join()`**

```c
void thr_join() {
  pthread_mutex_lock(&m)
  if (done == 0) {
    pthread_cond_wait(&c);
  }
  pthread_mutex_unlock(&m);
}
```

Esta versión introduce la variable de estado `done`, lo cual es correcto. Sin embargo, crea una condición de carrera (**race condition**) muy sutil porque no usa el mutex para proteger la variable done en ambos lados.

Ademas de lo anterior, tambien es posible que se de una perdida de la señal (Lost Wakeup).


### Implementación 3 - La implementación CORRECTA

**Codigo**: [implementacion3.c](implementacion3.c)

**Función `thr_exit()`**

```c
void thr_exit() {
  pthread_mutex_lock(&m);
  done = 1;
  pthread_cond_signal(&c);
  pthread_mutex_unlock(&m);
}
```

**Función `thr_join()`**

```c
void thr_join() {
  pthread_mutex_lock(&m)
  if (done == 0) {
    pthread_cond_wait(&c);
  }
  pthread_mutex_unlock(&m);
}
```

Esta implementación funciona porque el `mutex` protege la variable de estado `done` en todo momento.

> [!Tip]
> Adquiera siempre el lock (con `pthread_mutex_lock`) antes de llamar a `pthread_cond_wait` o `pthread_cond_signal`.

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Puede contener errores.