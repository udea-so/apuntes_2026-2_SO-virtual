![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Suma de vectores en Python (`threading`)

Versión en Python de la suma de vectores serial y paralela, traducida de los [programas en C](../c/README.md). Los conceptos (paralelismo de datos, speedup, división del trabajo, qué se mide y qué hace `Spin()`) están en la [guía principal](../README.md); esta guía se centra en cómo ejecutar los programas en Python, en las decisiones de la traducción y en los resultados obtenidos. La comparación con C está en la [guía principal](../README.md#comparación-c-vs-python).

---

## Requisitos

Todos los comandos se ejecutan desde esta carpeta (`sum_vectores/python/`).

```bash
python3 --version   # Python 3.6 o superior (por las f-strings)
free -h             # se necesitan ~1.2 GB libres
```

No se necesitan bibliotecas externas: `array`, `threading`, `os` y `time` son parte de la biblioteca estándar.

| Archivo | Equivale a | Contenido |
|---|---|---|
| `suma_s.py` | `suma_s.c` | Suma serial — línea base de tiempo (Ts) |
| `suma_p.py` | `suma_p.c` | Suma paralela con `threading` — tiempo Tp |
| `common.py` | `common.h` | `GetTime()` (con `time.time()`) y `Spin()` |

**Paciencia:** con `TAMANO = 100000000`, cada ejecución tarda del orden de **40 segundos** en la máquina de prueba: ~20 s de inicialización de los arreglos (fuera de la medición) y ~20 s o más de suma.

---

## Decisiones de la traducción

La traducción busca que los programas en Python hagan **exactamente lo mismo** que los de C, para que la comparación sea justa:

- **`array('i')` en lugar de listas.** El módulo estándar `array` guarda enteros de 4 bytes **contiguos** en memoria, como `malloc(TAMANO * sizeof(int))` en C. Una lista de Python, en cambio, guarda un puntero por elemento a un objeto entero independiente (de unas decenas de bytes cada uno), así que tres listas de 100 M elementos no cabrían en la máquina de prueba. Con `array('i')` la memoria máxima medida fue de **1.18 GB**, prácticamente igual a la de C (1.17 GB).
- **Misma inicialización.** `v1 = array('i', range(TAMANO))` y `v2 = array('i', range(0, 2 * TAMANO, 2))` dan `v1[i] = i` y `v2[i] = 2i`. `resultado` se crea lleno de ceros (`array('i', [0]) * TAMANO`), el equivalente a `resultado[i] = 0`: sus fallos de página ocurren antes de la medición, igual que en C.
- **Misma estructura de argumentos.** La clase `ThreadArgs` equivale al `struct thread_args_t`, y `sumar_porcion_vector` calcula la franja con las mismas fórmulas (con `//`, la división entera de Python).
- **Mismo número de hilos por defecto.** `os.sysconf("SC_NPROCESSORS_ONLN")` es la misma llamada al sistema que `sysconf(_SC_NPROCESSORS_ONLN)` en C. También se acepta el número de hilos como argumento: `python3 suma_p.py 2`.
- **Mismo `GetTime()` y `Spin()`.** `common.py` reproduce `common.h`, incluida la conversión del argumento de `Spin` a entero: `Spin(0.000001)` no espera nada y solo lee el reloj dos veces, como en C.
- **Una diferencia deliberada.** Dentro del ciclo, `sumar_porcion_vector` usa referencias locales a los arreglos en vez de `data.vector1[i]`: así cada iteración no busca el atributo en el objeto, y el ciclo de cada hilo es tan liviano como el de `suma_s.py`. Sin este cambio la versión paralela quedaría en desventaja por una razón ajena al paralelismo.

Se verificó que ambas versiones producen `resultado[i] == 3 * i` para todos los elementos, con 1, 3 y 8 hilos (incluido el caso en que el último hilo absorbe el residuo).

---

## Ejemplo 1 — Suma serial (`suma_s.py`)

### Ejecución

```bash
python3 suma_s.py
```

### Salida esperada

```
Ejecutando la suma en modo serial...
Tiempo de ejecución serial (Ts): 22.738363 segundos.
```

En la máquina de prueba Ts estuvo entre 22.1 y 23.3 segundos: unas **35 veces** más que la versión en C sin optimizar (0.64 s). Cada iteración de `resultado[i] = v1[i] + v2[i]` ejecuta varios bytecodes en el intérprete, y cada lectura de un `array` crea un objeto entero de Python a partir de los 4 bytes guardados.

---

## Ejemplo 2 — Suma paralela (`suma_p.py`)

### Ejecución

```bash
python3 suma_p.py        # un hilo por núcleo lógico
python3 suma_p.py 4      # número de hilos fijado a mano
```

### Salida esperada

```
Usando 8 hilos para la suma.
Suma paralela completada.
Tiempo de ejecución paralela (Tp): 27.634777 segundos.
```

El resultado es correcto, pero Tp es **mayor** que Ts: la versión "paralela" es más lenta que la serial. La sección siguiente muestra que esto ocurre con cualquier número de hilos y explica por qué.

---

## Resultados en la máquina de prueba

Para medir (ten en cuenta que cada ejecución tarda ~40 s con el tamaño por defecto):

```bash
for i in 1 2 3; do python3 suma_s.py | tail -1; done
for h in 1 2 4 8; do for i in 1 2 3; do echo -n "h=$h  "; python3 suma_p.py $h | tail -1; done; done
```

Medidas con tres ejecuciones por caso, con Python 3.8.10 (CPython estándar, con GIL), en un AMD Ryzen 5 3500U (4 núcleos físicos, 8 lógicos) bajo WSL1 — ver [entorno de ejecución](../../entorno_ejecucion.md) —, en la misma sesión que las mediciones de la [versión en C](../c/README.md). Se reporta la mediana; el speedup se calcula contra la mediana de Ts.

**Caso A — versión por defecto** (`TAMANO = 100000000`):

| Programa | Hilos | Tiempo (mediana) | Rango observado | Speedup |
|---|---|---|---|---|
| `suma_s.py` | 1 | 22.7 s | 22.1 – 23.3 s | 1.00 (Ts) |
| `suma_p.py` | 1 | 22.0 s | 18.6 – 22.8 s | ≈ 1.0 |
| `suma_p.py` | 2 | 36.0 s | 28.8 – 36.2 s | ≈ 0.6 |
| `suma_p.py` | 4 | 32.6 s | 30.1 – 41.4 s | ≈ 0.7 |
| `suma_p.py` | 8 | 27.9 s | 27.1 – 29.6 s | ≈ 0.8 |

**Caso B — compilación optimizada:** no aplica. Python no se compila a código de máquina; la opción `python3 -O` solo elimina los `assert` y no acelera este ciclo.

**Caso C — con `Spin()` activado en ambos programas** (`TAMANO = 1000000`):

| Programa | Hilos | Tiempo (mediana) | Rango observado | Speedup |
|---|---|---|---|---|
| `suma_s.py` | 1 | 1.89 s | 1.82 – 2.15 s | 1.00 (Ts) |
| `suma_p.py` | 1 | 2.01 s | 1.94 – 2.30 s | ≈ 0.9 |
| `suma_p.py` | 2 | 2.79 s | 2.42 – 2.99 s | ≈ 0.7 |
| `suma_p.py` | 4 | 2.90 s | 2.88 – 2.92 s | ≈ 0.7 |
| `suma_p.py` | 8 | 2.85 s | 2.54 – 3.23 s | ≈ 0.7 |

### Cómo interpretar los resultados

- **Más hilos no dan más velocidad: dan menos.** Con 2, 4 u 8 hilos la versión paralela tarda **más** que la serial, tanto en el caso A como en el C. Es el efecto del **GIL** (*Global Interpreter Lock*) de CPython: solo el hilo que tiene el GIL puede ejecutar bytecode de Python, así que los hilos no suman sus esfuerzos sino que **se turnan** el intérprete. El trabajo total es el mismo que en la versión serial, pero ahora repartido en turnos.
- **¿Por qué incluso más lento?** Además de no ganar nada, cada cambio de turno tiene un costo: el hilo que tiene el GIL debe soltarlo, el que espera debe despertarse y tomarlo, y los datos con los que trabaja pueden estar en la caché de otro núcleo. Con un solo hilo nada de esto ocurre, y por eso `suma_p.py` con 1 hilo tarda lo mismo que `suma_s.py`.
- **La prueba: el uso de CPU.** Con `/usr/bin/time` se puede ver cuántos núcleos usó el proceso en promedio (`%CPU`; 100 % equivale a un núcleo completo). En el caso C con 8 hilos, `suma_p.py` usó **99–100 %** de CPU en dos ejecuciones, lo mismo que con 1 hilo: aunque haya 8 hilos y 8 núcleos lógicos, en cada instante solo uno de ellos trabaja. La versión en C, con la misma prueba, usó ~450 % (ver la [guía de C](../c/README.md#cómo-interpretar-los-resultados)).

```bash
/usr/bin/time -f "%e s, %P CPU" python3 suma_p.py 8
```

- **No es un error del programa ni del concepto.** La partición del trabajo es correcta (el resultado es el mismo que en C) y en C la misma idea da un speedup de ≈ 4.4. Lo que cambia es la **implementación del lenguaje**: los hilos de CPython sirven para **concurrencia** (por ejemplo, superponer cálculos con esperas de red o disco, porque las operaciones de I/O sueltan el GIL), no para **paralelismo** de cálculos escritos en Python puro.

### ¿Cómo se paraleliza entonces en Python?

La documentación oficial del módulo `threading` recomienda, para aprovechar varios núcleos, usar **procesos** (`multiprocessing` o `concurrent.futures.ProcessPoolExecutor`): cada proceso tiene su propio intérprete y su propio GIL. El costo es que los procesos no comparten el espacio de direcciones, así que los datos deben copiarse entre ellos (pregunta 2). Otras rutas son las bibliotecas que hacen el cálculo fuera del intérprete y sueltan el GIL (como NumPy) y, desde Python 3.14, la variante oficial de CPython **sin GIL** (*free-threaded*, no instalada por defecto), en la que estos mismos hilos sí correrían en paralelo (pregunta 4). Más detalles en el [anexo C vs. Python](../../api-thread/anexo_c_python.md#una-advertencia-sobre-el-paralelismo-en-python).

---

## Preguntas para probar cambios

1. **Uso de CPU.** Ejecuta `suma_p.py` con 1, 2, 4 y 8 hilos bajo `/usr/bin/time -f "%e s, %P CPU"` (usa `TAMANO = 1000000` para que no tarde tanto). ¿Cambia el `%CPU` con el número de hilos? Haz lo mismo con `suma_p` de C. ¿Qué te dice cada número sobre cuántos hilos trabajan al mismo tiempo?
2. **Python con procesos.** Escribe una variante de `suma_p.py` que reparta las franjas con `concurrent.futures.ProcessPoolExecutor` en lugar de `threading`. Como cada proceso tiene su propio espacio de direcciones, los arreglos ya no se comparten: cada proceso debe recibir su franja de `v1` y `v2` y devolver su franja del resultado (usa `TAMANO = 10000000` para empezar). ¿Obtiene speedup? ¿Cuánto tiempo se va en copiar los datos entre procesos? ¿Por qué con hilos no hacía falta copiar nada? (Pista: compara con lo visto sobre espacios de direcciones de procesos e hilos.)
3. **Python con listas.** Cambia `array('i', ...)` por listas (`list(range(TAMANO))`, `[0] * TAMANO`) en `suma_s.py` con `TAMANO = 10000000` y mide con `/usr/bin/time -v` la memoria máxima (*Maximum resident set size*) de ambas versiones. ¿Cuántos bytes ocupa cada elemento en cada caso? ¿Por qué con listas no sería posible usar `TAMANO = 100000000` en la máquina de prueba? ¿Cambia también el tiempo?
4. **Python sin GIL.** Si tienes acceso a Python 3.13 o posterior en su variante *free-threaded* (el ejecutable suele llamarse `python3.13t` o `python3.14t`; verifica con `python3.14t -c "import sys; print(sys._is_gil_enabled())"`), repite el caso C con 1, 2, 4 y 8 hilos. ¿Aparece speedup? ¿Cómo se compara con C?
5. **Spin en Python.** En el caso C, cada `Spin()` llama dos veces a `time.time()`, y `suma_s.py` tarda ~1.9 s contra ~0.63 s de C con el mismo `TAMANO`. ¿De dónde sale la diferencia, si en ambos casos el trabajo pesado es leer el reloj? (Pista: cuenta cuántos bytecodes ejecuta una iteración en Python con `dis`, como en el [anexo C vs. Python](../../api-thread/anexo_c_python.md#ver-lo-que-no-es-atómico-objdump-en-c-dis-en-python).)

---

## Limpieza

```bash
rm -rf __pycache__      # la crea Python al importar common.py
```

---

## Referencias

- Python, módulo `array`: https://docs.python.org/3/library/array.html
- Python, módulo `threading` (incluye la nota sobre el GIL y la recomendación de usar procesos para aprovechar varios núcleos): https://docs.python.org/3/library/threading.html
- Python, módulo `concurrent.futures` (`ThreadPoolExecutor`, `ProcessPoolExecutor`): https://docs.python.org/3/library/concurrent.futures.html
- Python, *Python support for free threading* (CPython sin GIL): https://docs.python.org/3/howto/free-threading-python.html
- Python, `os.sysconf`: https://docs.python.org/3/library/os.html#os.sysconf

> [!note]
> **Nota sobre IA:** esta guía y los programas `suma_s.py`, `suma_p.py` y `common.py` fueron elaborados con asistencia de un modelo de inteligencia artificial. Los programas se ejecutaron y se verificó su resultado; los tiempos son mediciones reales en la máquina de prueba.
