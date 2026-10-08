![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Suma de vectores en C (`pthreads`)

Versión en C de la suma de vectores serial y paralela. Los conceptos (paralelismo de datos, speedup, división del trabajo, qué se mide y qué hace `Spin()`) están en la [guía principal](../README.md); esta guía se centra en cómo compilar y ejecutar los programas en C y en los resultados obtenidos. La comparación con Python está en la [guía principal](../README.md#comparación-c-vs-python).

---

## Requisitos

Todos los comandos se ejecutan desde esta carpeta (`sum_vectores/c/`).

```bash
gcc --version       # debe ser >= 7
free -h             # se necesitan ~1.2 GB libres
```

Los archivos `common.h` (`GetTime()` y `Spin()`) y `common_threads.h` deben estar en el mismo directorio que los fuentes. Son parte del repositorio de código del libro OSTEP.

| Archivo | Contenido |
|---|---|
| `suma_s.c` | Suma serial — línea base de tiempo (Ts) |
| `suma_p.c` | Suma paralela con `pthreads` — tiempo Tp |
| `common.h` | Header OSTEP: `GetTime()` (con `gettimeofday()`) y `Spin()` |
| `common_threads.h` | Header OSTEP: wrappers de `pthreads` que verifican errores (no usado aquí directamente) |

Si tienes unos **12 GB libres** (no basta con tenerlos instalados), puedes cambiar `TAMANO` a `1000000000` — en **ambos** archivos, para que la comparación sea válida.

---

## Detalles de la implementación en C

- Los arreglos se reservan con `malloc(TAMANO * sizeof(int))`: enteros de 4 bytes contiguos en el heap.
- Cada hilo recibe un puntero a su `thread_args_t`, un `struct` con los punteros a los tres arreglos, su `id`, `TAMANO` y `NUM_HILOS`. Como `pthread_create` acepta un solo `void *`, el `struct` es la forma de pasar varios datos (ver `t2` en [`api-thread/`](../../api-thread/README.md)).
- `NUM_HILOS` se obtiene con `sysconf(_SC_NPROCESSORS_ONLN)`, o del primer argumento: `./suma_p 2` usa 2 hilos sin importar cuántos núcleos haya.
- `GetTime()` usa `gettimeofday()` y devuelve un `double` en segundos con resolución de microsegundos.
- En `common.h`, `Spin(int howlong)` recibe un `int`: `Spin(0.000001)` se convierte en `Spin(0)` (ver la [guía principal](../README.md#simular-trabajo-más-pesado-spin)).

---

## Ejemplo 1 — Suma serial (`suma_s`)

### Compilación y ejecución

```bash
gcc -o suma_s suma_s.c -Wall
./suma_s
```

### Salida esperada

```
Ejecutando la suma en modo serial...
Tiempo de ejecución serial (Ts): 0.643106 segundos.
```

El tiempo varía según la máquina, la carga del sistema y las opciones de compilación. En la máquina de prueba, compilando como se indica arriba (sin optimización), Ts estuvo entre 0.49 y 0.65 segundos.

---

## Ejemplo 2 — Suma paralela (`suma_p`)

### Compilación y ejecución

```bash
gcc -o suma_p suma_p.c -lpthread -Wall
./suma_p        # un hilo por núcleo lógico
./suma_p 4      # número de hilos fijado a mano
```

### Salida esperada

```
Usando 8 hilos para la suma.
Suma paralela completada.
Tiempo de ejecución paralela (Tp): 0.147187 segundos.
```

El número de hilos depende de la máquina (la de prueba tiene 4 núcleos físicos y 8 lógicos).

---

## Resultados en la máquina de prueba

Para medir, repite cada caso varias veces (el tiempo de una sola ejecución puede variar bastante):

```bash
for i in 1 2 3; do ./suma_s | tail -1; done
for h in 1 2 4 8; do for i in 1 2 3; do echo -n "h=$h  "; ./suma_p $h | tail -1; done; done
```

Medidas con tres ejecuciones por caso en un AMD Ryzen 5 3500U (4 núcleos físicos, 8 lógicos) bajo WSL1 — ver [entorno de ejecución](../../entorno_ejecucion.md) —, en la misma sesión que las mediciones de la [versión en Python](../python/README.md). Se reporta la mediana; el speedup se calcula contra la mediana de Ts.

**Caso A — compilación sin optimización** (`gcc -Wall`, `TAMANO = 100000000`):

| Programa | Hilos | Tiempo (mediana) | Rango observado | Speedup |
|---|---|---|---|---|
| `suma_s` | 1 | 0.64 s | 0.49 – 0.65 s | 1.00 (Ts) |
| `suma_p` | 1 | 0.64 s | 0.60 – 0.83 s | ≈ 1.0 |
| `suma_p` | 2 | 0.29 s | 0.25 – 0.35 s | ≈ 2.2 |
| `suma_p` | 4 | 0.19 s | 0.17 – 0.40 s | ≈ 3.4 |
| `suma_p` | 8 | 0.15 s | 0.12 – 0.16 s | ≈ 4.4 |

**Caso B — compilación optimizada** (`gcc -O2 -Wall`, `TAMANO = 100000000`):

| Programa | Hilos | Tiempo (mediana) | Rango observado | Speedup |
|---|---|---|---|---|
| `suma_s` | 1 | 0.135 s | 0.125 – 0.140 s | 1.00 (Ts) |
| `suma_p` | 1 | 0.141 s | 0.132 – 0.142 s | ≈ 1.0 |
| `suma_p` | 2 | 0.118 s | 0.112 – 0.122 s | ≈ 1.1 |
| `suma_p` | 4 | 0.117 s | 0.114 – 0.117 s | ≈ 1.2 |
| `suma_p` | 8 | 0.113 s | 0.111 – 0.117 s | ≈ 1.2 |

**Caso C — con `Spin()` activado en ambos programas** (sin optimización, `TAMANO = 1000000`):

| Programa | Hilos | Tiempo (mediana) | Rango observado | Speedup |
|---|---|---|---|---|
| `suma_s` | 1 | 0.63 s | 0.62 – 0.63 s | 1.00 (Ts) |
| `suma_p` | 1 | 0.75 s | 0.63 – 0.80 s | ≈ 0.8 |
| `suma_p` | 2 | 0.39 s | 0.35 – 0.39 s | ≈ 1.6 |
| `suma_p` | 4 | 0.22 s | 0.20 – 0.23 s | ≈ 2.8 |
| `suma_p` | 8 | 0.15 s | 0.12 – 0.16 s | ≈ 4.1 |

### Cómo interpretar los resultados

- **Caso A: escala hasta los núcleos físicos.** Sin optimización, el ciclo ejecuta muchas instrucciones por elemento (el compilador lee y escribe `i` y los punteros en memoria en cada vuelta), así que el trabajo está limitado por la CPU. Duplicar los hilos casi divide el tiempo a la mitad hasta llegar a 4, el número de núcleos **físicos**. Pasar de 4 a 8 hilos ayuda menos: los hilos extra corren en los núcleos lógicos de SMT, que comparten unidades de ejecución con los primeros 4. Una de las ejecuciones con 4 hilos tardó 0.40 s, más del doble que las otras dos: el ruido de medición es real, y por eso se reporta la mediana.
- **Caso B: el cuello de botella es la memoria.** Con `-O2` el ciclo se reduce a unas pocas instrucciones por elemento y un solo hilo ya es capaz de pedir datos casi tan rápido como la memoria RAM los puede entregar. Cada elemento implica leer 8 bytes y escribir 4, unos 1.2 GB de tráfico por ejecución: en 0.135 s (un hilo) eso son ~9 GB/s, y en 0.113 s (8 hilos) ~11 GB/s, valores cercanos a lo que la memoria de esta laptop puede sostener en la práctica. Agregar hilos casi no ayuda porque todos compiten por el mismo bus de memoria. A esto se le llama un problema **limitado por memoria** (*memory-bound*).
- **Caso C: trabajo de CPU.** Con `Spin()` cada elemento cuesta dos llamadas a `gettimeofday()`, mucho más que la suma misma, y el acceso a memoria deja de importar. El speedup crece con cada duplicación de hilos, y aquí los 8 hilos sí aportan sobre los 4 (≈ 4.1 contra ≈ 2.8). En una sesión anterior en la misma máquina este caso llegó a ≈ 3.5 con 4 hilos y ≈ 5.4 con 8: la tendencia es la misma, pero las cifras exactas cambian entre sesiones.
- `suma_p` con 1 hilo tarda prácticamente lo mismo que `suma_s` (sus rangos se solapan): crear un hilo cuesta microsegundos, nada comparado con décimas de segundo de trabajo.
- **Uso de CPU.** Con `/usr/bin/time` se puede ver cuántos núcleos usó el proceso en promedio (`%CPU`; 100 % equivale a un núcleo completo). En el caso C con 8 hilos, `suma_p` usó **~450 %** de CPU (dos ejecuciones: 446 % y 451 %), frente a ~100 % con 1 hilo: los hilos de C corren de verdad en varios núcleos a la vez. Compáralo con la [versión en Python](../python/README.md#cómo-interpretar-los-resultados).

```bash
/usr/bin/time -f "%e s, %P CPU" ./suma_p 8
```

El speedup real siempre será menor que el ideal por varias razones: la parte serial del programa (en este ejemplo queda fuera de la medición, pero en un programa real cuenta), el overhead de crear y esperar hilos, la competencia por el ancho de banda de memoria y por las cachés compartidas, y los núcleos lógicos que no son núcleos físicos completos.

---

## Preguntas para probar cambios

1. **Escalabilidad.** Mide `suma_s` y `suma_p` con 1, 2, 4, 8 y 16 hilos (3 ejecuciones cada uno) y llena una tabla como las de arriba. Grafica el speedup contra el número de hilos junto con la recta ideal `speedup = hilos`. ¿A partir de cuántos hilos deja de mejorar? ¿Coincide con el número de núcleos físicos o lógicos de tu máquina (`lscpu`)? ¿Qué pasa con 16 hilos, más que núcleos lógicos?
2. **Optimización del compilador.** Repite la pregunta 1 compilando ambos programas con `-O2`. ¿Por qué baja Ts tanto? ¿Por qué baja tanto el speedup? Calcula los GB/s que logra cada versión (tráfico ≈ 12 bytes × `TAMANO` / tiempo) y compáralos entre sí.
3. **Carga de CPU.** Reduce `TAMANO` a `1000000` y descomenta `Spin(0.000001);` **en los dos archivos**. ¿Por qué hay que reducir `TAMANO`? Repite las mediciones. ¿Cómo cambia la curva de speedup respecto a la pregunta 1? Lee la definición de `Spin` en `common.h`: ¿cuánto espera realmente cada llamada? ¿Qué pasaría si `Spin` recibiera un `double` y esperara de verdad 1 µs por elemento con `TAMANO = 100000000`?
4. **Fallos de página.** Quita la línea `resultado[i] = 0;` de la inicialización en los dos archivos y vuelve a medir. ¿Cuánto cambian Ts y Tp? ¿Cambia el speedup? Explica la diferencia usando lo visto sobre paginación y asignación de marcos bajo demanda.

---

## Limpieza

```bash
rm -f suma_s suma_p
```

---

## Referencias

- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*. Cap. 27 (*Thread API*): https://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf
- Repositorio de código y headers OSTEP: https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-api
- `pthread_create(3)`, `pthread_join(3)`, `sysconf(3)`, `gettimeofday(2)` — páginas de manual de Linux (`man pthread_create`, etc.).

> [!note]
> **Nota sobre IA:** esta guía fue elaborada con asistencia de un modelo de inteligencia artificial. El código fue compilado y ejecutado para verificar su corrección.
