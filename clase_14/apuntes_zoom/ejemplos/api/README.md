![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Ejemplos de la API de hilos: repaso rápido

Repaso de los ejemplos de la API POSIX de hilos que se trabajaron en clase: cómo compilarlos, cómo ejecutarlos y qué salida esperar. La explicación de cada concepto está en el [apunte de la clase](../../README.md); la guía completa, con preguntas para modificar el código y las versiones en Python, está en [`clase_14/ejemplos/api-thread/`](../../../ejemplos/api-thread/README.md).

| Archivo | Concepto | En clase |
|---|---|---|
| [`t0.c`](t0.c) | Creación de hilos y orden de ejecución no determinista | Se compiló y ejecutó (15/10) |
| [`t1.c`](t1.c) | Datos compartidos y *race condition* | Se compiló y ejecutó (20/10) |
| [`t2.c`](t2.c) | Argumentos y valor de retorno con `struct` | Se compiló y ejecutó (20/10) |
| [`t3.c`](t3.c) | Paso de un valor simple en el `void *` | Solo se explicó (diapositivas y comentarios del código) |

> [!NOTE]
> **Sobre las salidas.** Son de una ejecución de referencia hecha el 22/10/2026 en la máquina del profesor (WSL1 sobre Windows 10, `gcc` 9.4.0, 8 CPU lógicas; ver [entorno de ejecución](../../../ejemplos/entorno_ejecucion.md)), no de la sesión de clase. Las direcciones de memoria, el orden de algunas líneas y los valores de `counter` cambian en cada ejecución: eso es justamente lo que muestran estos ejemplos.

Requisitos: Linux (o WSL) con `gcc`. Todos los comandos se ejecutan desde esta carpeta.

## `t0`: orden de ejecución

`main` crea dos hilos que imprimen "A" y "B" y los espera con `pthread_join`.

```sh
gcc -o t0 t0.c -lpthread -Wall
./t0
```

Salida habitual:

```
main: begin
A
B
main: end
```

Pero el orden de `A` y `B` no está garantizado. Al ejecutarlo 1000 veces seguidas, 995 corridas dieron la salida anterior y 5 dieron esta:

```
main: begin
B
A
main: end
```

Lo único garantizado es que `main: begin` sale primero y `main: end` al final, porque `main` imprime este último después de los dos `pthread_join`.

Para repetir el experimento (cuenta cuántas veces sale cada orden):

```sh
for i in $(seq 1000); do ./t0 | tr '\n' ' '; echo; done | sort | uniq -c
```

## `t1`: datos compartidos

Dos hilos incrementan `max` veces cada uno la variable global `counter`. El valor correcto al final es `max * 2`.

```sh
gcc -o t1 t1.c -lpthread -Wall
./t1 <iteraciones>
```

Con pocas iteraciones el resultado suele ser correcto: cada hilo termina antes de que el otro empiece a competir por `counter`.

```
$ ./t1 1000
main: begin [counter = 0] [7FA7FB7D302C]
A: begin [addr of i: 0x7fa7fb54fedc]
A: done
B: begin [addr of i: 0x7fa7fad3fedc]
B: done
main: done
 [counter: 2000]
 [should: 2000]
$ ./t1 10000
main: begin [counter = 0] [7FD60B47202C]
A: begin [addr of i: 0x7fd60b1efedc]
A: done
B: begin [addr of i: 0x7fd60a9dfedc]
B: done
main: done
 [counter: 20000]
 [should: 20000]
```

Desde 100 000 iteraciones los dos hilos corren al mismo tiempo (fíjese en que `B: begin` aparece antes de `A: done`) y se pierden incrementos:

```
$ ./t1 100000
main: begin [counter = 0] [7F5AC365B02C]
A: begin [addr of i: 0x7f5ac33dfedc]
B: begin [addr of i: 0x7f5ac2bcfedc]
A: done
B: done
main: done
 [counter: 136114]
 [should: 200000]
```

Con 10 000 000, como en clase, tres ejecuciones seguidas dieron tres valores distintos, todos lejos de 20 000 000:

```
$ ./t1 10000000
main: begin [counter = 0] [7F0FBA91C02C]
A: begin [addr of i: 0x7f0fba69fedc]
B: begin [addr of i: 0x7f0fb9e8fedc]
A: done
B: done
main: done
 [counter: 11436147]
 [should: 20000000]
$ ./t1 10000000
main: begin [counter = 0] [7FDE2A2D902C]
A: begin [addr of i: 0x7fde2a05fedc]
B: begin [addr of i: 0x7fde2984fedc]
A: done
B: done
main: done
 [counter: 11583311]
 [should: 20000000]
$ ./t1 10000000
main: begin [counter = 0] [7F7A92FDF02C]
A: begin [addr of i: 0x7f7a92d4fedc]
B: begin [addr of i: 0x7f7a9253fedc]
B: done
A: done
main: done
 [counter: 12073092]
 [should: 20000000]
```

Qué observar:

* La dirección de `counter` (entre corchetes en la primera línea) es una sola: la variable es compartida.
* La dirección de `i` es distinta para `A` y para `B`: cada hilo tiene su propia `i` en su propio *stack*.
* En la tercera ejecución `B` terminó antes que `A`: el orden tampoco está garantizado.

## `t2`: argumentos y retorno con `struct`

`main` pasa al hilo un `struct` con `a = 10` y `b = 20`; el hilo los imprime y devuelve otro `struct`, reservado con `malloc`, con `x = 1` y `y = 2`.

```sh
gcc -o t2 t2.c -lpthread -Wall
./t2
```

```
10 20
returned 1 2
```

La primera línea la imprime el hilo y la segunda `main`, después de `pthread_join`. Aquí el orden sí está garantizado: `main` no imprime hasta que el hilo termina.

## `t3`: valor simple

> [!NOTE]
> Este ejemplo no se ejecutó en clase: se explicó con la diapositiva "Pasando un valor simple a un hilo" y con los comentarios del código. La versión de la diapositiva usa `int`, que es incorrecta en 64 bits; `t3.c` usa `long long int`, que tiene el mismo tamaño que `void *`.

El hilo recibe el número 100 directamente en el `void *`, lo imprime y devuelve 101 de la misma forma.

```sh
gcc -o t3 t3.c -lpthread -Wall
./t3
```

```
100
returned 101
```

## Limpieza

```sh
rm -f t0 t1 t2 t3
```
