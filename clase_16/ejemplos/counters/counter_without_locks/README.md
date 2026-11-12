![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Contadores — Parte 1: contador sin lock

Primer eslabón de la cadena de contadores: qué ocurre cuando varios hilos incrementan una variable compartida sin ningún mecanismo de protección. Corresponde a las diapositivas p. 5–7 y a OSTEP §29.1 (Fig. 29.1, *A Counter Without Locks*).

Las salidas son de una ejecución de referencia del 11/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14 (WSL1, `gcc` 9.4.0, AMD Ryzen 5 3500U con 4 núcleos físicos y 8 lógicos). Los valores cambian de una ejecución a otra y de una máquina a otra; lo que debe repetirse es el patrón.

---

## Archivos

| Archivo | Contenido |
|---|---|
| [`counter.h`](counter.h) / [`counter.c`](counter.c) | El contador de las diapositivas (p. 5): `counter_t` con un solo campo `value` y las funciones `init`, `increment`, `decrement` y `get`, ninguna protegida. |
| [`counter_without_locks.c`](counter_without_locks.c) | Programa de la diapositiva p. 6: 8 hilos incrementan un millón de veces el mismo contador. Genera `bad_counter`. |
| [`test_counter_cli.c`](test_counter_cli.c) | La misma prueba, con el número de hilos y de incrementos por hilo como argumentos. Genera `counter_test`. |
| [`Makefile`](Makefile) | Compila los dos ejecutables. |
| [`pruebas_contador_sin_locks.ipynb`](pruebas_contador_sin_locks.ipynb) | Gráficas de la tabla de las diapositivas (p. 7). |

`GetTime()`, que mide el tiempo transcurrido, está en [`../include/common.h`](../include/common.h) (tomado de OSTEP).

---

## Por qué se pierden incrementos

`c->value++` parece una sola operación, pero el compilador la traduce a tres instrucciones: leer `value` de memoria a un registro, sumarle 1 y escribir el registro de vuelta en memoria. Si dos hilos leen el mismo valor antes de que alguno escriba, los dos escriben el mismo resultado y uno de los incrementos se pierde:

```
value = 5
Hilo 1: lee value      -> registro = 5
Hilo 2: lee value      -> registro = 5
Hilo 2: suma y escribe -> value = 6
Hilo 1: suma y escribe -> value = 6     <- se esperaba 7
```

Es la *race condition* de las clases 14 y 15. El cuerpo de `increment()` es una región crítica sin protección, así que el contador **no es thread-safe** (diapositivas p. 5).

---

## Compilación

```bash
make
```

Equivale a:

```bash
gcc -Wall -I../include counter_without_locks.c counter.c -pthread -o bad_counter
gcc -Wall -I../include test_counter_cli.c counter.c -pthread -o counter_test
```

---

## Ejemplo 1 — `bad_counter` (8 hilos, un millón de incrementos cada uno)

```bash
./bad_counter
```

Salida de referencia:

```
-> Tiempo gastado: 0.079232 seg
-> El contador debe quedar en: 8000000
-> El valor real del contador es: 1784315
```

En cinco ejecuciones más el valor fue 1270989, 1981546, 1357916, 1574297 y 1779339. Se perdió entre el 75 % y el 84 % de los incrementos, y la cantidad perdida cambia en cada ejecución: la race condition no es determinista, porque depende de cómo se intercalen los hilos.

En la clase (diapositiva p. 6) la misma prueba dio 1162851: otra ejecución, otro valor.

---

## Ejemplo 2 — `counter_test` (hilos e incrementos por argumento)

```bash
./counter_test <num_threads> <count_per_thread>
```

Con un solo hilo no hay con quién competir y el resultado es exacto:

```bash
./counter_test 1 1000000
```

```
Iniciando prueba con 1 hilos, 1000000 incrementos cada uno.

-> Tiempo gastado: 0.007736 seg
-> El contador debe quedar en: 1000000
-> El valor real del contador es: 1000000
```

El número de incrementos se pasa a cada hilo dentro del `void *` usando `intptr_t`, como en el ejemplo `t3` de la [clase 14](../../../../clase_14/ejemplos/api-thread/README.md).

### Experimento: variar el número de hilos

```bash
for t in 1 2 4 8 16; do ./counter_test $t 1000000; done
```

Resultados de referencia (un millón de incrementos por hilo):

| Hilos | Valor esperado | Valor obtenido | Tiempo (s) |
|---|---|---|---|
| 1 | 1000000 | 1000000 | 0.0075 |
| 2 | 2000000 | 1109598 | 0.0207 |
| 4 | 4000000 | 1352352 | 0.0435 |
| 8 | 8000000 | 1407391 | 0.0875 |
| 16 | 16000000 | 1437830 | 0.1710 |

Es la misma tabla de las diapositivas (p. 7), medida en la máquina de referencia: con dos o más hilos el valor obtenido queda muy por debajo del esperado. Los valores y los tiempos no coinciden con los de las diapositivas porque son otra ejecución en otra máquina.

### Experimento: variar el número de incrementos

```bash
for n in 1000 10000 100000 1000000; do ./counter_test 4 $n; done
```

| Incrementos por hilo (4 hilos) | Valor esperado | Valor obtenido |
|---|---|---|
| 1000 | 4000 | 4000 |
| 10000 | 40000 | 40000 |
| 100000 | 400000 | 269022 |
| 1000000 | 4000000 | 1446974 |

Con pocos incrementos, cada hilo termina su ciclo antes de que arranque el siguiente y no llegan a coincidir; el resultado es correcto por casualidad, no porque el código lo sea. Con más incrementos los hilos sí se superponen y la carrera aparece.

---

## Notebook

```bash
jupyter notebook pruebas_contador_sin_locks.ipynb
```

Grafica los datos de la tabla de las diapositivas (p. 7): valor esperado vs. obtenido, porcentaje de incrementos perdidos y tiempo. Los datos están escritos en el propio notebook, así que no depende de ejecutar los programas.

---

## Limpieza

```bash
make clean
```

---

## Conclusión

El contador sin lock es rápido, pero con más de un hilo el resultado es incorrecto. La solución es proteger la región crítica con un lock: es el contador preciso de la [Parte 2](../precise_counter/README.md).

---

## Referencias

- Arpaci-Dusseau, R. y Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*, v1.10. Cap. 29, *Lock-based Concurrent Data Structures*, §29.1 [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf), y cap. 26, *Concurrency: An Introduction* (la race condition) [[pdf]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf).
- `man pthread_create`, `man pthread_join`.

> [!IMPORTANT]
> **Sobre esta guía.** El código y esta guía se elaboraron con asistencia de un modelo de inteligencia artificial y se revisaron contra las diapositivas de la clase y OSTEP; todos los programas se compilaron y ejecutaron para obtener las salidas de referencia. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente; se agradece reportarla en los foros del curso.
