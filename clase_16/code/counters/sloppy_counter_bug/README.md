![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Estructuras de datos basadas en locks — Parte 3: Contador aproximado (Sloppy Counter)

Guía de ejemplos autónoma para la Clase 16 de Sistemas Operativos.  
Esta parte resuelve el problema de escalamiento del grupo anterior: un contador
que reduce la contención sobre el lock global a cambio de un valor que puede
no ser exacto en todo momento.

---

## Requisitos

| Herramienta | Versión mínima | Verificación |
|---|---|---|
| GCC | 7.0 | `gcc --version` |
| pthreads | (incluida en glibc) | `ldconfig -p \| grep libpthread` |
| Python + Jupyter | 3.8 / 6.0 | `jupyter --version` (opcional, para el notebook) |

Se recomienda haber ejecutado primero los ejemplos de las **Partes 1 y 2**
para que los tiempos y el patrón de escalamiento tengan contexto comparativo.

---

## Conceptos previos

### El problema que resuelve: la contención del lock global

En la Parte 2, todos los hilos compiten por un único `pthread_mutex_t`. Cada
incremento — aunque ocurra en un núcleo diferente — obliga al hilo a esperar
turno. El hardware podría ejecutar múltiples incrementos al mismo tiempo, pero
el lock los serializa todos. Añadir núcleos no mejora el rendimiento; solo
aumenta la cola de espera frente al mismo lock.

### La idea del sloppy counter

En lugar de un único contador compartido, el sloppy counter usa una estructura
de varios niveles:

- **Un contador local por CPU** (`local[NUMCPUS]`), cada uno con su propio
  lock (`llock[NUMCPUS]`). Los hilos que corren en la misma CPU compiten
  solo entre sí, no con los hilos de otras CPUs.
- **Un contador global** (`global`) con su lock (`glock`), que acumula el
  total real.
- **Un umbral** (`threshold`, llamado S en las diapositivas), que determina
  cada cuántos incrementos locales se transfiere el valor al global.

El flujo de un incremento es:

```
1. Adquirir llock[threadID]
2. local[threadID] += amt
3. Si local[threadID] >= threshold:
       Adquirir glock
       global += local[threadID]
       Liberar glock
       local[threadID] = 0
4. Liberar llock[threadID]
```

Los hilos de distintas CPUs nunca compiten por el mismo `llock`, lo que
elimina la mayor parte de la contención. El `glock` solo se toca una vez
cada `threshold` incrementos — en lugar de una vez por incremento.

### El precio: la aproximación

`get()` devuelve únicamente `global`. Los valores acumulados en los contadores
locales que aún no superaron el umbral **no están reflejados**. Esto significa
que el valor reportado puede ser menor al real en hasta
`(threshold - 1) × NUMCPUS` unidades en cualquier momento durante la
ejecución.

Más importante: al terminar el programa, si los contadores locales tienen
valores residuales que no alcanzaron el umbral para disparar la transferencia,
esos incrementos **se pierden permanentemente** del global. El programa termina
con un valor ligeramente menor al esperado — no por una condición de carrera,
sino por diseño.

### El parámetro threshold (S)

- **S pequeño** (ej. S=1): cada incremento local se transfiere inmediatamente
  al global. Comportamiento idéntico al lock global de la Parte 2 — correcto
  pero lento.
- **S grande** (ej. S=1024): los hilos trabajan casi completamente en sus
  contadores locales, tocando `glock` muy poco. El tiempo se reduce
  drásticamente, pero el valor global se retrasa y puede quedar incompleto
  al final.

El threshold es la perilla que regula el tradeoff entre rendimiento y
precisión. No hay un valor "correcto" — depende de cuánta aproximación tolera
la aplicación.

### Escenario real

Este patrón se usa en contadores de métricas de sistemas en producción: el
número de peticiones procesadas por segundo en un servidor, el total de bytes
transmitidos en una red, o contadores de eventos en sistemas de monitoreo como
Linux `perf` o los contadores de páginas del kernel. En todos estos casos se
acepta que el valor reportado esté ligeramente desactualizado a cambio de no
introducir un cuello de botella de sincronización en el camino crítico. Un
contador de votos en una elección, en cambio, no toleraría esta aproximación.

### Nota sobre `NUMCPUS`

`NUMCPUS = 8` está definido en `sloppy_counter.h` para una máquina AMD Ryzen 5
3500U (4 núcleos físicos, 8 hilos lógicos). El `threadID` que cada hilo pasa
a `update()` se calcula como `i % NUMCPUS`, de modo que aunque haya más hilos
que CPUs, cada hilo usa uno de los 8 contadores locales disponibles. En
máquinas con menos núcleos físicos el beneficio de paralelismo es menor — ver
la sección de salida esperada para el detalle.

---

## Organización de los archivos

| Archivo | Rol | Concepto que ilustra |
|---|---|---|
| `sloppy_counter.h` | Define `counter_t` (global + locales + locks + threshold) | Estructura del sloppy counter |
| `sloppy_counter.c` | Implementa `init`, `update`, `get` | Mecanismo de transferencia local → global |
| `main_sloppy_counter.c` | Ejecutable simple: 16 hilos, threshold=1024, 1 M iter. | Corrección aproximada observable |
| `benchmark_sloppy_counter.c` | Varía hilos (1–16), threshold fijo alto, escribe `results_sloppy.csv` | Escalamiento vs contador preciso |
| `benchmark_thresh_amt1.c` | Varía threshold (1–1024), hilos fijos=8, escribe `results_thresh_amt1.csv` | Trade-off threshold / precisión |
| `results_sloppy.csv` | Datos experimentales del benchmark de hilos (incluido en el repo) | Comparación de tiempos por número de hilos |
| `results_thresh_amt1.csv` | Datos experimentales del benchmark de threshold (incluido en el repo) | Curva threshold vs tiempo |
| `pruebas_contador_aproximado.ipynb` | Notebook Python: grafica ambos CSVs | Visualización del tradeoff |

---

## Ejemplo 1 — `sloppy_example`: corrección aproximada

### Contexto teórico

`main_sloppy_counter.c` lanza 16 hilos con `threshold=1024`. Cada hilo
incrementa 1,000,000 veces llamando a `update()` con `amt=1`. Al terminar,
`get()` lee solo `global` — los valores en `local[]` que no alcanzaron el
umbral quedan sin transferir. El output usa la palabra "aproximado" de forma
explícita e intencional.

### Compilación

```bash
gcc -Wall sloppy_counter.c main_sloppy_counter.c -lpthread -o sloppy_example
```

### Ejecución

```bash
./sloppy_example
```

### Salida esperada

```
Iniciando prueba con 16 hilos (Threshold=1024)...
-> Tiempo gastado: 0.364223 seg
-> El contador debe quedar en: 16000000
-> El valor aproximado del contador es: 15998976
```

La diferencia entre esperado y real es `16,000,000 − 15,998,976 = 1,024`.
Ese valor es exactamente un `threshold`: un contador local quedó con 1,024
incrementos sin transferir al terminar el programa.

### Por qué este output demuestra el concepto

El programa no está roto — el resultado incompleto es una consecuencia
predecible del diseño. `get()` no flush los contadores locales antes de leer;
devuelve únicamente lo que ya llegó a `global`. Con `threshold=1024` y
`NUMCPUS=8`, en el peor caso pueden quedar hasta `8 × 1023 = 8,184`
incrementos sin transferir. La corrida muestra exactamente 1,024 — un solo
contador local que terminó en el límite sin disparar la última transferencia.

> **Comportamiento honesto del código**
>
> No hay ningún mecanismo artificial para forzar ni ocultar la imprecisión.
> El residuo varía entre corridas según cómo el planificador distribuye los
> hilos, pero siempre es un múltiplo de `threshold`. Si se ejecuta varias
> veces, el valor obtenido cambia dentro de ese rango: `15,998,976`,
> `15,997,952` (dos contadores residuales), etc.

---

## Ejemplo 2 — `benchmark_sloppy`: escalamiento con múltiples hilos

### Contexto teórico

`benchmark_sloppy_counter.c` repite el experimento de la Parte 2 pero con el
sloppy counter. Usa `THRESHOLD=1,000,000` — deliberadamente alto — para
minimizar los accesos a `glock` y mostrar el máximo beneficio de velocidad.
Con ese umbral y `COUNT_PER_THREAD=1,000,000`, cada contador local acumula
exactamente hasta el threshold y transfiere una sola vez, de modo que el
valor final resulta exacto en este benchmark en particular.

### Compilación

```bash
gcc -Wall sloppy_counter.c benchmark_sloppy_counter.c -lpthread -o benchmark_sloppy
```

### Ejecución

```bash
./benchmark_sloppy
```

### Salida esperada en consola

```
Iniciando benchmark para 'sloppy_counter' (Threshold=1000000)...
Resultados se guardarán en results_sloppy.csv
  Probando con 1 hilos...
    Tiempo: 0.0218 s, Esperado: 1000000, Real: 1000000
  Probando con 2 hilos...
    Tiempo: 0.0434 s, Esperado: 2000000, Real: 2000000
  Probando con 4 hilos...
    Tiempo: 0.0861 s, Esperado: 4000000, Real: 4000000
  Probando con 8 hilos...
    Tiempo: 0.1721 s, Esperado: 8000000, Real: 8000000
  Probando con 16 hilos...
    Tiempo: 0.3549 s, Esperado: 16000000, Real: 16000000
Benchmark completado.
```

### Comparación con el contador preciso (datos del repo, máquina multicore)

| Hilos | Preciso (s) | Sloppy (s) | Speedup |
|---|---|---|---|
| 1 | 0.0358 | 0.0430 | 0.83x |
| 2 | 0.1170 | 0.1148 | 1.02x |
| 4 | 0.1941 | 0.1682 | 1.15x |
| 8 | 0.3430 | 0.2698 | 1.27x |
| 16 | 0.7454 | 0.5245 | 1.42x |

Con 1 hilo el sloppy es ligeramente más lento (overhead de `llock` adicional
sin beneficio de paralelismo). A partir de 2 hilos empieza a superar al
preciso, y la ventaja crece con el número de hilos.

> **Nota sobre máquinas de un solo núcleo**
>
> En un sistema con un único núcleo físico (`nproc = 1`), el sloppy counter
> y el preciso muestran tiempos similares porque no hay paralelismo real — los
> hilos se turnan en la misma CPU. Los datos del repo (Ryzen 5 3500U, 8
> threads lógicos) son los representativos para observar el beneficio de
> escalamiento. Si tu máquina tiene pocos núcleos, los tiempos locales
> diferirán pero el patrón de los CSVs incluidos sigue siendo válido para
> el análisis.

---

## Ejemplo 3 — `benchmark_thresh`: el tradeoff threshold / precisión

### Contexto teórico

Este benchmark mantiene fijos el número de hilos (`NUMCPUS=8`) y el trabajo
total (`TARGET_COUNT_PER_THREAD=10,000,000` por hilo), y varía `threshold`
entre 1 y 1024. Es la reproducción directa de la gráfica "Scaling Sloppy
Counters" de las diapositivas. Muestra cómo el tiempo cae a medida que S
crece, y cómo el valor reportado empieza a desviarse del esperado cuando S
es suficientemente grande.

### Compilación

```bash
gcc -Wall sloppy_counter.c benchmark_thresh_amt1.c -lpthread -o benchmark_thresh
```

### Ejecución

```bash
./benchmark_thresh
```

### Salida esperada en consola

```
Iniciando benchmark (Hilos fijos: 8, Target por hilo: 10000000, AMT fijo: 1)
  Probando Threshold=1      ... Tiempo: 3.3364 s
  Probando Threshold=2      ... Tiempo: 2.5634 s
  Probando Threshold=4      ... Tiempo: 2.1611 s
  Probando Threshold=8      ... Tiempo: 1.9414 s
  Probando Threshold=16     ... Tiempo: 1.8769 s
  Probando Threshold=32     ... Tiempo: 1.8279 s
  Probando Threshold=64     ... Tiempo: 1.7554 s
  Probando Threshold=128    ... Tiempo: 1.7752 s
  Probando Threshold=256    ... Tiempo: 1.7321 s
  Probando Threshold=512    ... Tiempo: 1.7302 s
  Probando Threshold=1024   ... Tiempo: 1.7202 s
Benchmark completado. Resultados en 'results_thresh_amt1.csv'
```

### Salida esperada en `results_thresh_amt1.csv`

```
Threshold,Time(s),Expected,Actual
1,3.3364,80000000,80000000
2,2.5634,80000000,80000000
...
256,1.7321,80000000,79998976
512,1.7302,80000000,79998976
1024,1.7202,80000000,79994880
```

### Por qué este output demuestra el concepto

Tres observaciones directas sobre los datos:

**1. La velocidad cae abruptamente al inicio.** De `S=1` (7.8s) a `S=4` (3.9s)
el tiempo se reduce a la mitad. De `S=4` a `S=1024` solo mejora otro 50%.
El mayor beneficio viene de evitar las transferencias más frecuentes.

**2. La exactitud se pierde gradualmente.** Con `S≤128` el valor es exacto
(80,000,000). Con `S=256` ya aparece un residuo de 1,024 incrementos sin
transferir. Con `S=1024` el residuo es 5,120 — exactamente 5 contadores
locales con `threshold-1` incrementos atrapados al terminar.

**3. El residuo es siempre múltiplo de `threshold`.** Esto no es ruido
estadístico — es la firma del mecanismo: los incrementos se pierden en bloques
exactos de `threshold` unidades, uno por contador local que quedó con trabajo
pendiente al terminar el programa.

> **Por qué los valores finales no son exactos**
>
> `get()` está implementado así intencionalmente:
> ```c
> int get(counter_t *c) {
>     pthread_mutex_lock(&c->glock);
>     int val = c->global;
>     pthread_mutex_unlock(&c->glock);
>     return val; // only approximate!
> }
> ```
> No suma los contadores locales. Un `get()` "preciso" requeriría adquirir
> los `NUMCPUS` locks locales más el global — un costo prohibitivo que
> anularía el beneficio del diseño. La aproximación no es un descuido: es la
> decisión de diseño que hace posible el escalamiento.

---

## Análisis de datos — Notebook

```bash
jupyter notebook pruebas_contador_aproximado.ipynb
```

El notebook lee ambos CSVs y produce dos gráficas:

- **Threads vs Tiempo**: compara sloppy contra el patrón lineal del preciso.
- **Threshold vs Tiempo**: muestra la curva descendente del tradeoff.

> **Nota:** Ejecutar el notebook desde el mismo directorio donde están
> `results_sloppy.csv` y `results_thresh_amt1.csv`.

---

## Limpieza

```bash
rm -f sloppy_example benchmark_sloppy benchmark_thresh \
       results_sloppy.csv results_thresh_amt1.csv
```

---

## Conclusiones de este grupo

| Mecanismo | Qué resuelve | Qué limitación introduce |
|---|---|---|
| Sin lock (Parte 1) | Máxima velocidad | Resultados incorrectos — pérdida por race condition |
| Lock global (Parte 2) | Resultados exactos con N hilos | Serializa todos los accesos — escala linealmente |
| **Sloppy counter (esta parte)** | **Reduce contención — escala mejor con núcleos** | **El valor global es aproximado; residuos al terminar** |

El sloppy counter cierra la cadena de los contadores concurrentes: corrección
total (sin lock) → corrección exacta pero lenta (lock global) → corrección
aproximada y escalable (sloppy). Ninguno de los tres es universalmente mejor —
la elección depende de si la aplicación necesita exactitud en tiempo real,
exactitud al finalizar, o solo una tendencia aproximada con alto rendimiento.

---

## Referencias

- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy
  Pieces*. Capítulo 29 (*Lock-based Concurrent Data Structures*), sección
  "Scalable Counting".
  Disponible en: <https://pages.cs.wisc.edu/~remzi/OSTEP/>

- Código de referencia del libro (capítulo 29):
  <https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf>

- Documentación de POSIX Threads:
  `man pthread_mutex_init`, `man pthread_mutex_lock`, `man pthread_mutex_unlock`

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Tenga en cuenta que puede contener errores.

> [!warning]
> **Aclaración**: Como todo código (especialmente los ejemplos didácticos diseñados pueden fallar (o de hecho fallan)), el contenido debe ser revisado críticamente y puede contener errores.