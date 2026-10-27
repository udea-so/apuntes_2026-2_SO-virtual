![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Locks — Guía de ejemplos

**Clase 15 - Sistemas Operativos**

Esta guía permite explorar los conceptos de sincronización entre hilos ejecutando código real. Los ejemplos recorren la evolución completa: desde el problema que origina la necesidad de los locks hasta las primitivas de hardware que los hacen posibles.

---

## Requisitos

| Herramienta | Verificación |
|---|---|
| GCC | `gcc --version` |
| Python 3 | `python3 --version` |
| pthreads | incluido en Linux por defecto |

> Probado en Linux (Ubuntu 22.04+).

---

## Conceptos previos

Antes de ejecutar los ejemplos conviene tener claros los siguientes conceptos. Si alguno no es familiar, la sección de referencias al final de esta guía indica dónde profundizar.

### Hilos y memoria compartida

Un **hilo** (thread) es una unidad de ejecución dentro de un proceso. A diferencia de los procesos, los hilos de un mismo proceso **comparten el espacio de memoria**: el código, el heap y las variables globales son visibles para todos. Cada hilo tiene su propia pila (stack) y sus propios registros.

```
Proceso
├── Código         ← compartido por todos los hilos
├── Heap           ← compartido por todos los hilos
├── Variables glob ← compartidas por todos los hilos
├── Stack hilo A   ← privado de A
└── Stack hilo B   ← privado de B
```

Esa memoria compartida es lo que permite la comunicación entre hilos, pero también es la fuente de todos los problemas de sincronización.

### Sección crítica

Una **sección crítica** es cualquier fragmento de código que accede a un recurso compartido — típicamente una variable o estructura de datos — y cuya ejecución debe ser **exclusiva**: si un hilo la está ejecutando, ningún otro debería poder hacerlo al mismo tiempo.

```c
// Esto es una sección crítica:
counter = counter + 1;   // lee, modifica y escribe una variable compartida
```

### Race condition (condición de carrera)

Una **race condition** ocurre cuando el resultado de un programa depende del orden relativo en que los hilos ejecutan sus instrucciones, y ese orden no está controlado. El resultado es **no determinista**: el programa puede producir valores diferentes en cada ejecución aunque el código sea idéntico.

La causa más común es que una operación que parece atómica en el código fuente se traduce a múltiples instrucciones de ensamblador:

```c
counter = counter + 1;
```

Se compila a:

```asm
mov 0x<addr>, %eax     ; (1) lee counter desde memoria al registro
add $0x1,     %eax     ; (2) incrementa el registro
mov %eax,     0x<addr> ; (3) escribe el resultado en memoria
```

Si el scheduler interrumpe un hilo entre (1) y (3) y otro hilo ejecuta sus tres pasos completos, la escritura del segundo hilo se pierde cuando el primero ejecuta (3) con su valor desactualizado. Una actualización se pierde.

### Exclusión mutua

La **exclusión mutua** es la garantía de que si un hilo está ejecutando una sección crítica, ningún otro hilo puede entrar a ella hasta que el primero salga. Es la propiedad fundamental que los locks implementan.

### Lock

Un **lock** (también llamado mutex, de *mutual exclusion*) es un objeto en memoria con dos estados posibles:

- **libre** (`free`): ningún hilo lo tiene, cualquiera puede tomarlo.
- **tomado** (`held`): un hilo lo tiene y está en la sección crítica.

Y dos operaciones:

- `acquire()` / `lock()`: espera hasta que el lock esté libre y lo toma.
- `release()` / `unlock()`: libera el lock para que otro hilo pueda tomarlo.

El patrón de uso es siempre:

```c
acquire(lock);
    sección crítica   // solo un hilo aquí a la vez
release(lock);
```

### Instrucciones atómicas

Una **instrucción atómica** es una instrucción de hardware que ejecuta múltiples pasos (leer, modificar, escribir) como una unidad indivisible — no hay punto intermedio donde otro hilo pueda interrumpir. Son el soporte de hardware que hace posible implementar locks correctamente.

Las dos más importantes en x86:

- **`xchg`** (exchange): intercambia el valor de un registro con el de una posición de memoria de forma atómica. Retorna el valor anterior. Base del *test-and-set*.
- **`cmpxchg`** (compare-and-exchange): compara el valor de una posición de memoria con un valor esperado y, solo si coinciden, lo reemplaza con un nuevo valor. Retorna un flag de éxito. Base del *compare-and-swap*.

### Spinlock

Un **spinlock** es la implementación más simple de un lock usando instrucciones atómicas. Cuando el lock está tomado, el hilo que espera **gira** en un loop revisando la bandera continuamente hasta que se libere. Es correcto pero ineficiente: consume ciclos de CPU sin hacer trabajo útil.

### yield

`yield()` es una llamada al sistema que mueve el hilo que la llama de estado **running** a estado **ready**, cediendo voluntariamente la CPU al scheduler. Usado dentro del loop de espera de un spinlock, evita desperdiciar el quantum completo girando sin hacer nada.

---

## Organización de los ejemplos

```
Bloque 1 — El problema          t1_nolock.c   /  t1_nolock.py
Bloque 2 — La solución          t1_lock.c     /  t1_lock.py
Bloque 3 — Cómo funciona        test-and-set.c / spinlock.c / compare-and-swap.c
Bloque 4 — Mejora del spinlock  spinlock_yield.c
```

---

## Bloque 1 — El problema: race condition

Dos hilos incrementan un contador compartido. Sin protección, algunas actualizaciones se pierden por la race condition descrita en los conceptos previos.

### `t1_nolock.c`

```bash
gcc -O0 -o t1_nolock t1_nolock.c -lpthread -Wall
./t1_nolock 1000
```

Salida esperada:
```
main: begin [counter = 0]
A: done
B: done
main: done
[counter: 1001]      <- incorrecto (debería ser 2000)
[should:  2000]
```

Ejecutar varias veces muestra que el valor varía entre corridas. `sched_yield()` en el código fuerza el cambio de contexto entre los pasos (1) y (3) para que la race condition sea siempre observable. En producción ocurre igual pero de forma no determinista.

### `t1_nolock.py`

```bash
python3 t1_nolock.py 100
```

Salida esperada:
```
main: begin [counter = 0]
A: done
B: done
main: done
[counter: 100]       <- incorrecto (debería ser 200)
[should:  200]
```

El problema es el mismo en Python. `time.sleep(0)` libera el GIL de CPython entre los pasos (1) y (3). Sin esa línea el GIL protege accidentalmente la operación y la race condition no aparece, aunque el código sea igualmente incorrecto.

> El GIL (Global Interpreter Lock) es un mutex interno de CPython que garantiza que solo un hilo ejecuta bytecode Python a la vez. Protege la integridad del intérprete pero no es un mecanismo de sincronización para el programador — no debe dependerse de él.

> Usar 100 iteraciones y no 1000000 porque `time.sleep(0)` hace cada iteración más lenta.

---

## Bloque 2 — La solución: locks

Los mismos dos hilos incrementando el mismo contador, ahora con un lock protegiendo la sección crítica.

### `t1_lock.c`

```bash
gcc -o t1_lock t1_lock.c -lpthread -Wall
./t1_lock 1000000
```

Salida esperada:
```
main: begin [counter = 0]
A: done
B: done
main: done
 [counter: 2000000]
 [should: 2000000]
```

El código de los hilos es idéntico al de `t1_nolock.c` salvo por `pthread_mutex_lock` y `pthread_mutex_unlock`. Esas dos llamadas son todo lo que separa un programa con race condition de uno correcto.

### `t1_lock.py`

```bash
python3 t1_lock.py 1000000
```

`with lock:` en Python es el equivalente de `pthread_mutex_lock / pthread_mutex_unlock`. El context manager garantiza que el lock siempre se libera aunque ocurra una excepción dentro de la sección crítica.

---

## Bloque 3 — Cómo se construye un lock

`pthread_mutex_t` es una caja negra. Este bloque la abre y muestra que por dentro hay instrucciones atómicas de hardware que garantizan que leer-modificar-escribir ocurra sin interrupciones posibles.

### `test-and-set.c` — la primitiva `xchg`

Demuestra la instrucción `xchg` de forma aislada sobre una variable global.

```bash
gcc -o test-and-set test-and-set.c -Wall
./test-and-set
```

Salida esperada:
```
before acquiring lock: 0
after  acquiring lock: 1 (old: 0)                <- estaba libre (0), retorna 0
before acquiring lock (already held): 1
after  acquiring lock (already held): 1 (old: 1) <- estaba tomado (1), retorna 1
before releasing lock: 1
after  releasing lock: 0 (old: 1)                <- libera, retorna valor anterior
```

`xchg` intercambia `*ptr` con `new_val` y retorna el valor anterior, todo en una sola instrucción atómica. No necesita el prefijo `lock` explícito porque x86 lo garantiza implícitamente para cualquier `xchg` con operando en memoria.

### `spinlock.c` — spinlock sobre test-and-set

Construye un spinlock usando la estructura de `lock_t` y demuestra su ciclo completo: init, acquire, intento fallido, release, acquire de nuevo.

```bash
gcc -o spinlock spinlock.c -Wall
./spinlock
```

Salida esperada:
```
before lock:  flag = 0
after  lock:  flag = 1 (lock adquirido)

before spin attempt:  flag = 1
after  spin attempt:  flag = 1 (old: 1) -> lock ocupado, seguir girando

before unlock: flag = 1
after  unlock: flag = 0 (lock liberado)

before lock:  flag = 0
after  lock:  flag = 1 (lock adquirido de nuevo)
```

La cadena completa de abstracción que este archivo hace visible:

```
lock()  →  TestAndSet()  →  xchg  (hardware)
```

La correctitud no depende de `lock()` ni de `TestAndSet()` sino de que `xchg` sea atómica. Si se reemplazara `xchg` por dos instrucciones separadas equivalentes, se volvería al mismo problema del Bloque 1.

### `compare-and-swap.c` — la primitiva `cmpxchg`

Demuestra la instrucción `cmpxchg` de forma aislada: una operación exitosa y una fallida.

```bash
gcc -o compare-and-swap compare-and-swap.c -Wall
./compare-and-swap
```

Salida esperada:
```
before successful cas: 0
after successful cas: 100 (success: 1)  <- valor coincidía: actualiza, retorna 1
before failing cas: 100
after failing cas: 100 (success: 0)     <- valor no coincidía: no actualiza, retorna 0
```

Diferencia respecto a `xchg`:

| | `xchg` (test-and-set) | `cmpxchg` (compare-and-swap) |
|---|---|---|
| Escribe | Siempre | Solo si el valor coincide con el esperado |
| Retorna | Valor anterior | Flag de éxito (0 o 1) |
| Tráfico en bus | En cada intento | Solo cuando tiene éxito |
| Prefijo `lock` | Implícito en x86 | Debe declararse explícitamente |

---

## Bloque 4 — Mejora del spinlock: yield

El spinlock puro consume todo el quantum de CPU girando sin hacer nada útil. Con N hilos esperando se desperdician N−1 quanta completos. `yield()` resuelve esto moviendo el hilo de **running → ready** y cediendo la CPU voluntariamente.

```
Sin yield:  O(threads × time_slice)   <- desperdicia CPU
Con yield:  O(threads × ctx_switch)   <- más eficiente
```

### `spinlock_yield.c`

```bash
gcc -o spinlock_yield spinlock_yield.c -Wall
./spinlock_yield
```

Salida esperada:
```
before lock:  flag = 0
after  lock:  flag = 1 (lock adquirido)

before yield attempt:  flag = 1
after  yield attempt:  flag = 1 (old: 1) -> lock ocupado, yield()

before unlock: flag = 1
after  unlock: flag = 0 (lock liberado)

before lock:  flag = 0
after  lock:  flag = 1 (lock adquirido de nuevo)
```

El único cambio respecto a `spinlock.c`:

```c
// spinlock.c
while (TestAndSet(&lock->flag, 1) == 1)
    ;           // spin-wait (do nothing)

// spinlock_yield.c
while (TestAndSet(&lock->flag, 1) == 1)
    yield();    // give up the CPU
```

`yield()` mejora el desempeño pero no elimina la posibilidad de **inanición**: el scheduler puede seguir eligiendo hilos que no tienen el lock. La solución a ese problema son los queue locks con `park/unpark`, donde el SO mantiene una cola explícita de hilos en espera y garantiza que eventualmente todos sean atendidos.

---

## Limpieza

```bash
rm -f compare-and-swap test-and-set spinlock spinlock_yield t1_nolock t1_lock
```

---

## Conclusiones

Los ejemplos de esta guía recorren la cadena completa que existe detrás de `pthread_mutex_lock()`:

```
pthread_mutex_lock()        nivel de aplicación
        ↓
spinlock_lock() / yield()   nivel de biblioteca
        ↓
TestAndSet()                nivel de primitiva
        ↓
xchg / cmpxchg              nivel de hardware (ISA)
```

Cada mecanismo resuelve el problema del anterior e introduce uno nuevo:

| Mecanismo | Resuelve | Limitación que introduce |
|---|---|---|
| Load/Store simple | — | No garantiza exclusión mutua |
| test-and-set (`xchg`) | Exclusión mutua | Spinlock gasta CPU |
| Spinlock + yield | Reduce gasto de CPU | Puede causar inanición |
| Queue lock (park/unpark) | Elimina inanición | Mayor complejidad |
| `pthread_mutex_t` | Todo lo anterior | Dependencia del SO |

La conclusión central es que **la atomicidad no viene del software sino del hardware**. Sin `xchg` o `cmpxchg`, ninguna solución en software puede garantizar exclusión mutua de forma correcta y eficiente en un sistema multiprocesador.

---

## Referencias

### Libro de texto

- **Arpaci-Dusseau, R. & Arpaci-Dusseau, A.** — *Operating Systems: Three Easy Pieces* (OSTEP), disponible gratuitamente en [ostep.org](https://ostep.org).
  - Capítulo 26 — *Concurrency: An Introduction* — hilos, memoria compartida y el problema del contador.
  - Capítulo 27 — *Thread API* — uso de `pthread_create`, `pthread_join`, `pthread_mutex_t`.
  - Capítulo 28 — *Locks* — implementación de locks, test-and-set, compare-and-swap, ticket lock, yield y queue locks.

### Diapositivas del curso

- **Clase 15 — Locks** · Sistemas Operativos · Universidad de Antioquia. Disponibles en el repositorio del curso.

### Documentación de APIs

- **pthreads (C)**
  - `man pthread_create` / `man pthread_mutex_lock` — manual del sistema.
  - [POSIX Threads Programming — LLNL](https://hpc-tutorials.llnl.gov/posix/) — guía completa con ejemplos.

- **threading (Python)**
  - [threading — Thread-based parallelism](https://docs.python.org/3/library/threading.html) — documentación oficial.
  - [threading.Lock](https://docs.python.org/3/library/threading.html#threading.Lock) — referencia del objeto Lock.

- **sched.h (C)**
  - `man sched_yield` — documentación de la llamada al sistema yield en Linux.

- **GIL de CPython**
  - [What is the Python GIL?](https://realpython.com/python-gil/) — explicación accesible del Global Interpreter Lock.

### Código fuente de referencia

- [ostep-code/threads-locks](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-locks) — código oficial del capítulo 28 de OSTEP.
- [xv6 spinlock](https://github.com/mit-pdos/xv6-public/blob/master/spinlock.c) — implementación real de spinlock en el kernel educativo xv6 del MIT.

---

> [!note]
> **Nota sobre IA:** esta guía fue elaborada con asistencia de un modelo de inteligencia artificial. El código fue compilado y ejecutado para verificar su corrección.

> [!warning]
> * Algunos ejemplos usan mecanismos artificiales (`sched_yield`, `time.sleep(0)`) para hacer observable un comportamiento que en producción ocurre de forma no determinista. Los resultados deben interpretarse en ese contexto.
> * La IA puede cometer errores. Compruebe su precisión.