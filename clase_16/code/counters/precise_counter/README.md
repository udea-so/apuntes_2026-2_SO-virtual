![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Estructuras de datos basadas en locks — Parte 2: Contador preciso con mutex

Guía de ejemplos autónoma para la Clase 16 de Sistemas Operativos.  
Esta parte resuelve el problema del grupo anterior: un contador compartido
que produce resultados correctos con cualquier número de hilos, a cambio
de un costo medible en rendimiento.

---

## Requisitos

| Herramienta | Versión mínima | Verificación |
|---|---|---|
| GCC | 7.0 | `gcc --version` |
| pthreads | (incluida en glibc) | `ldconfig -p \| grep libpthread` |
| Python + Jupyter | 3.8 / 6.0 | `jupyter --version` (opcional, para el notebook) |

Se recomienda haber ejecutado primero los ejemplos de la **Parte 1**
(`counter_without_locks`) para que la comparación de tiempos tenga contexto.

---

## Conceptos previos

### El patrón lock / región crítica / unlock

La solución al problema de la condición de carrera es envolver la región
crítica con un mecanismo de exclusión mutua. En POSIX Threads ese mecanismo
es `pthread_mutex_t`. El patrón tiene tres pasos:

```
pthread_mutex_lock(&lock);
    /* región crítica — solo un hilo a la vez */
pthread_mutex_unlock(&lock);
```

Cuando un hilo llama a `pthread_mutex_lock` y el mutex está libre, lo adquiere
y continúa. Si otro hilo ya lo tiene, el hilo que intenta adquirirlo queda
bloqueado (sale de la CPU) hasta que el propietario llame a
`pthread_mutex_unlock`. El sistema operativo despierta entonces a uno de los
hilos en espera.

### Por qué el resultado es ahora exacto

Con el mutex protegiendo `value++`, la secuencia LOAD → ADD → STORE ocurre de
forma atómica desde el punto de vista de los demás hilos: ninguno puede
interponerse entre esas tres instrucciones porque el mutex les impide entrar
a la región crítica. No se pierden incrementos.

### El costo: serialización

La corrección tiene un precio. Con un único mutex global sobre el contador,
todos los hilos compiten por el mismo recurso — solo uno puede incrementar
en cada momento. En la práctica, los hilos no trabajan en paralelo sino en
fila. Esto tiene dos consecuencias:

- **El tiempo crece linealmente (o peor) con el número de hilos.** Añadir
  más hilos no acelera el programa; en cambio, añade contención sobre el lock.
- **Incluso con un solo hilo, el programa es más lento** que la versión sin
  lock, porque cada incremento paga el costo de adquirir y liberar el mutex.

### Escenario real

Este patrón es el que usa cualquier contador de acceso seguro en producción:
el número de conexiones activas en un servidor web, el saldo disponible en
una cuenta bancaria compartida entre múltiples sesiones, o el stock de un
producto durante una venta flash. El mutex garantiza que ninguna actualización
se pierda, pero si miles de hilos compiten por el mismo lock simultáneamente,
el servidor se convierte en un cuello de botella. Ese problema — correcto
pero no escalable — es exactamente lo que motiva el contador aproximado
de la Parte 3.

---

## Organización de los archivos

| Archivo | Rol | Concepto que ilustra |
|---|---|---|
| `counter_lock.h` | Define `counter_t` con `value` + `lock`, declara funciones | Estructura thread-safe |
| `counter_lock.c` | Implementa `init`, `increment`, `decrement`, `get` con mutex | Patrón lock/unlock alrededor de la región crítica |
| `main_precise_counter.c` | Ejecutable simple: 16 hilos, 1 M iteraciones, una corrida | Verificación de corrección |
| `benchmark_precise_counter.c` | Ejecutable que prueba 1–16 hilos y escribe `results_lock.csv` | Análisis de escalamiento |
| `results_lock.csv` | Datos experimentales del benchmark (incluido en el repo) | Comparación de tiempos por número de hilos |
| `pruebas_contador_preciso.ipynb` | Notebook Python: lee el CSV y grafica Threads vs Tiempo | Visualización del poor scaling |

---

## Ejemplo 1 — `precise_counter`: verificación de corrección

### Contexto teórico

`main_precise_counter.c` es el equivalente directo de `bad_counter` del grupo
anterior: lanza 16 hilos, cada uno incrementa 1,000,000 veces, y al final
imprime esperado vs real. La única diferencia estructural es que `counter_t`
ahora incluye un `pthread_mutex_t lock` y cada operación lo adquiere antes
de tocar `value`. El resultado debe ser siempre exacto,
sin importar cuántas veces se ejecute.

### Compilación

```bash
gcc -Wall counter_lock.c main_precise_counter.c -lpthread -o precise_counter
```

### Ejecución

```bash
./precise_counter
```

### Salida esperada

```
-> Tiempo gastado: 0.368377 seg
-> El contador debe quedar en: 16000000
-> El valor real del contador es: 16000000
```

El valor real siempre coincide con el esperado. El tiempo varía según el
hardware, pero será notablemente mayor que el de `bad_counter` con el mismo
número de hilos.

### Por qué este output demuestra el concepto

La coincidencia entre esperado y real es determinista — no es suerte ni
depende de la velocidad del hardware. Puede ejecutarse cien veces y el
resultado es siempre correcto. Eso es thread-safety. El precio visible es el
tiempo: en el entorno del curso, 16 hilos tardan ~0.37s aquí contra ~0.09s
en `bad_counter` con 8 hilos. El programa es más lento y más correcto.

---

## Ejemplo 2 — `benchmark_precise_counter`: análisis de escalamiento

### Contexto teórico

`benchmark_precise_counter.c` automatiza la comparación que en la Parte 1 se
hacía manualmente con el loop `for t in 1 2 4 8 16`. Prueba cada configuración
de hilos, mide el tiempo, verifica que el resultado sea correcto, y escribe
todo en `results_lock.csv`. El notebook lee ese archivo y produce la gráfica
de escalamiento.

El pipeline completo es:

```
compilar → ejecutar benchmark → CSV → notebook → gráfica
```

Cada paso depende del anterior. El CSV incluido en el repositorio corresponde
a una ejecución real en la máquina del curso; los tiempos en tu máquina serán
distintos pero el patrón de escalamiento será el mismo.

### Compilación

```bash
gcc -Wall counter_lock.c benchmark_precise_counter.c -lpthread -o benchmark_precise_counter
```

### Ejecución

```bash
./benchmark_precise_counter
```

### Salida esperada en consola

```
Iniciando benchmark para 'counter_lock'...
Resultados se guardarán en results_lock.csv
  Probando con 1 hilos...
    Tiempo: 0.0224 s, Esperado: 1000000, Real: 1000000
  Probando con 2 hilos...
    Tiempo: 0.0436 s, Esperado: 2000000, Real: 2000000
  Probando con 4 hilos...
    Tiempo: 0.0875 s, Esperado: 4000000, Real: 4000000
  Probando con 8 hilos...
    Tiempo: 0.1705 s, Esperado: 8000000, Real: 8000000
  Probando con 16 hilos...
    Tiempo: 0.3473 s, Esperado: 16000000, Real: 16000000
Benchmark completado.
```

Los tiempos variarán, pero Esperado y Real deben coincidir en todas las filas.

### Salida esperada en `results_lock.csv`

```
Threads,Time(s),Expected,Actual
1,0.0224,1000000,1000000
2,0.0436,2000000,2000000
4,0.0875,4000000,4000000
8,0.1705,8000000,8000000
16,0.3473,16000000,16000000
```

Los datos del CSV incluido en el repo muestran tiempos distintos (máquina
del curso, 1 hilo → 0.0358s, 16 hilos → 0.7454s). Ambos conjuntos exhiben
el mismo patrón: el tiempo crece aproximadamente en proporción al número de
hilos — evidencia directa de la serialización impuesta por el mutex global.

### Por qué este output demuestra el concepto

Con un contador sin lock, más hilos no garantizan más trabajo útil — solo más
pérdidas. Con el mutex, más hilos sí hacen más trabajo total (16 × 1 M =
16 M incrementos reales), pero el tiempo crece proporcionalmente porque el
lock los obliga a turnarse. El ratio T(16) / T(1) ≈ 15–21x en distintas
máquinas — prácticamente lineal. En un contador que escalara perfectamente,
ese ratio debería ser cercano a 1x (16 hilos terminarían en el mismo tiempo
que 1 hilo pero haciendo 16x más trabajo). El mutex hace imposible ese ideal.

---

## Análisis de datos — Notebook

```bash
jupyter notebook pruebas_contador_preciso.ipynb
```

El notebook lee `results_lock.csv` y produce una gráfica de Threads vs Tiempo.
La curva resultante es casi lineal — cada hilo adicional añade
aproximadamente el mismo costo. Esto contrasta directamente con la gráfica
del sloppy counter (Parte 3), donde la curva es plana.

> **Nota:** Ejecutar el notebook desde el mismo directorio donde está
> `results_lock.csv`, o ajustar la variable `csv_path` en la primera celda.

---

## Limpieza

```bash
rm -f precise_counter benchmark_precise_counter results_lock.csv
```

---

## Conclusiones de este grupo

| Mecanismo | Qué resuelve | Qué limitación introduce |
|---|---|---|
| Sin lock (Parte 1) | Máxima velocidad | Resultados incorrectos con más de un hilo |
| **Lock global (esta parte)** | **Resultados exactos con N hilos** | **Serializa todos los accesos — escala linealmente con los hilos** |
| *(siguiente: sloppy counter)* | Escala bien en múltiples núcleos | El valor global es aproximado, no exacto en tiempo real |

El contador con mutex resuelve la corrección completamente, pero convierte un
problema paralelizable en uno secuencial. Esa es la tensión fundamental que
da lugar al sloppy counter: ¿es posible obtener corrección suficiente sin
pagar el costo de serialización total?

---

## Referencias

- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy
  Pieces*. Capítulo 28 (*Locks*) y Capítulo 29 (*Lock-based Concurrent Data
  Structures*, sección "Concurrent Counters").
  Disponible en: <https://pages.cs.wisc.edu/~remzi/OSTEP/>

- Documentación de POSIX Threads:
  `man pthread_mutex_init`, `man pthread_mutex_lock`, `man pthread_mutex_unlock`

- Código de referencia del libro (capítulo 29):
  <https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf>

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Tenga en cuenta que puede contener errores.

> [!warning]
> **Aclaración**: Como todo código (especialmente los ejemplos didácticos diseñados pueden fallar (o de hecho fallan)), el contenido debe ser revisado críticamente y puede contener errores.