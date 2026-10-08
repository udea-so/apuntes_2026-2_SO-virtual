![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Anexo — Hilos en C (`pthreads`) y en Python (`threading`)

Los ejemplos de esta carpeta existen en dos lenguajes: el código en C es el original del libro (OSTEP) y el código en Python es su traducción. La idea es mostrar que **los conceptos no dependen del lenguaje**: crear un hilo, esperar a que termine, pasarle datos, compartir memoria, el no-determinismo y la race condition existen igual en ambos. Lo que cambia es la **API**: el nombre de las funciones, cómo se pasan los argumentos y cómo se devuelven los resultados. Cada lenguaje (Java, Rust, Go, C++, ...) tiene la suya, pero detrás de todas está la misma abstracción que ofrece el sistema operativo.

Este anexo compara las dos versiones. Supone que ya leíste la [guía principal](README.md).

---

## 1. Tabla de equivalencias de la API

| Concepto | C (`pthreads`) | Python (`threading`) | Observación |
|---|---|---|---|
| Usar la biblioteca | `#include <pthread.h>` y compilar con `-lpthread` | `import threading` | En Python es parte de la biblioteca estándar; no hay paso de compilación |
| Representar un hilo | `pthread_t p;` | Un objeto `threading.Thread` | |
| Código que ejecuta el hilo | `void *mythread(void *arg)` | `def mythread(arg):` | En C la firma es fija; en Python la función recibe los parámetros que se le pasen |
| Crear el hilo | `pthread_create(&p, NULL, mythread, arg);` | `p = threading.Thread(target=mythread, args=(arg,))` | En C, crear un hilo también lo pone a correr |
| Iniciar el hilo | (incluido en `pthread_create`) | `p.start()` | En Python crear e iniciar son dos pasos separados |
| Pasar varios argumentos | Empaquetarlos en un `struct` y pasar su dirección como `void *` | Ponerlos en la tupla `args=(a, b, ...)` o pasar un objeto | `pthread_create` acepta **un solo** argumento |
| Esperar a que termine | `pthread_join(p, NULL);` | `p.join()` | Ambos bloquean al hilo que llama hasta que `p` termine |
| Recibir un valor de retorno | `pthread_join(p, (void **) &r);` recibe el `void *` que devolvió el hilo | `join()` devuelve `None`: se usa un objeto compartido (lista, atributo) o `concurrent.futures` | Ver la sección 3 |
| `main` termina sin hacer `join` | El proceso termina y los hilos que no han acabado mueren con él | El intérprete **espera** a los hilos antes de salir, salvo los creados con `daemon=True` | Ver la sección 3 |
| Variable compartida | Variable global (`volatile int counter`) | Atributo de un objeto global (`counter.value`) | En Python, asignar a un nombre global dentro de una función exige `global`; un objeto mutable evita ese detalle |
| Exclusión mutua (§27.3) | `pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;` y `pthread_mutex_lock(&lock)` / `pthread_mutex_unlock(&lock)` | `lock = threading.Lock()` y `with lock:` | El `with` libera el lock aunque ocurra una excepción |
| Memoria dinámica | `malloc` / `free` manuales | Automática (recolector de basura) | |
| Errores | Las funciones devuelven un código (`rc`) que hay que revisar | Se lanzan excepciones | |
| Identificar el hilo actual | `pthread_self()` | `threading.get_ident()` o `threading.current_thread().name` | |

---

## 2. Comparación por ejemplo

Cada tabla pone lado a lado el código de los dos archivos. Para que quepa en dos columnas, algunas líneas largas se partieron en varias y algunos comentarios se movieron de línea; la lógica es idéntica a la de los archivos de la carpeta. Bajo cada tabla se indica qué se omitió.

### `t0` — Creación de hilos y orden no determinista

<table>
<tr><th>C — <code>t0.c</code></th><th>Python — <code>t0.py</code></th></tr>
<tr>
<td>

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *mythread(void *arg) {
    printf("%s\n", (char *) arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "usage: main\n");
        exit(1);
    }

    pthread_t p1, p2;
    printf("main: begin\n");
    // create two threads
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: end\n");
    return 0;
}
```

</td>
<td>

```python
import threading
import sys

def mythread(arg):
  print(arg)

if __name__ == "__main__":
  if len(sys.argv) != 1:
    print("usage: main\n")
  else:
    print("main: begin")
    # Creacion de los hilos
    p1 = threading.Thread(target=mythread,
                          args=('A',))
    p2 = threading.Thread(target=mythread,
                          args=('B',))
    # Se inicia la ejecucion de los hilos
    p1.start()
    p2.start()
    # El hilo padre espera que los hilos
    # hijos culminen
    p1.join()
    p2.join()
    print("main: end")
```

</td>
</tr>
</table>

| Mismo concepto | Diferencia de API |
|---|---|
| Dos hilos ejecutan la misma función con argumentos distintos | `pthread_create` crea e inicia; en Python, `Thread(...)` crea y `.start()` inicia |
| `join` espera a cada hilo antes de imprimir `main: end` | En C se pasa el `pthread_t`; en Python se llama al método del objeto |
| El orden de `A` y `B` lo decide el scheduler | En C el argumento llega como `void *` y hay que convertirlo (`(char *) arg`); en Python llega con su tipo |

(Se omitieron los `#include` y el comentario de compilación que no aportan a la comparación; en `t0.py` también se omitieron un `import time` y un `time.sleep(3)` comentado.)

### `t1` — Datos compartidos y race condition

<table>
<tr><th>C — <code>t1.c</code></th><th>Python — <code>t1.py</code></th></tr>
<tr>
<td>

```c
int max;
volatile int counter = 0; // shared global variable

void *mythread(void *arg) {
    char *letter = arg;
    int i; // stack (private per thread)
    printf("%s: begin [addr of i: %p]\n", letter, &i);
    for (i = 0; i < max; i++) {
        counter = counter + 1; // shared: only one
    }
    printf("%s: done\n", letter);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: t1 <loopcount>\n");
        exit(1);
    }
    max = atoi(argv[1]);

    pthread_t p1, p2;
    printf("main: begin [counter = %d] [%lX]\n",
           counter, (long unsigned int) &counter);
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: done\n [counter: %d]\n [should: %d]\n",
           counter, max*2);
    return 0;
}
```

</td>
<td>

```python
import threading
import sys

# Necesario usar un objeto para que sea
# mutable y compartido entre hilos
class Counter:
    def __init__(self):
        self.value = 0

counter = Counter()
max = 0

def mythread(arg):
  global counter
  # stack (private per thread)
  print(f"{arg}: begin")
  for i in range(max):
    counter.value += 1
  print(f"{arg}: done")

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("usage: t1.py <loopcount>")
  else:
    max = int(sys.argv[1])
    print(f"main: begin [counter = {counter.value}]")
    p1 = threading.Thread(target=mythread, args=('A',))
    p2 = threading.Thread(target=mythread, args=('B',))
    p1.start()
    p2.start()
    p1.join()
    p2.join()
    print(f"main: done")
    print(f" [counter: {counter.value}]\n"
          f" [should: {max*2}]")
```

</td>
</tr>
</table>

| Mismo concepto | Diferencia de API o de lenguaje |
|---|---|
| `counter` es **uno solo** para los dos hilos; `i` es **privada** de cada hilo | En C, `i` vive en el stack del hilo y se puede imprimir su dirección (`&i`); Python no expone direcciones de memoria, así que `t1.py` no puede mostrar esa evidencia, aunque cada llamada a `mythread` sigue teniendo su propia `i` |
| El incremento no es atómico: en C son tres instrucciones de máquina (`mov`/`add`/`mov`), en Python varios bytecodes (leer el atributo, sumar, escribir el atributo) | `volatile` no existe en Python: el intérprete siempre lee y escribe el atributo `value` del objeto |
| Se pierden actualizaciones en ambos lenguajes (ver la tabla de `t1` en la [guía principal](README.md)) | En CPython el GIL hace que nunca haya dos hilos ejecutando bytecode a la vez; aun así la carrera aparece porque el cambio de hilo puede ocurrir entre los bytecodes del incremento |

(En `t1.c` se omitieron los `#include`; en `t1.py`, los comentarios de los `start`/`join`, idénticos a los de `t0.py`.)

### `t2` — Argumentos y valores de retorno

<table>
<tr><th>C — <code>t2.c</code></th><th>Python — <code>t2.py</code></th></tr>
<tr>
<td>

```c
typedef struct __myarg_t {
  int a;
  int b;
} myarg_t;

typedef struct __myret_t {
  int x;
  int y;
} myret_t;

void *mythread(void *arg) {
  myarg_t *m = (myarg_t *) arg;
  printf("%d %d\n", m->a, m->b);
  // allocated in heap (Dont forget to free it)
  myret_t *r = malloc(sizeof(myret_t));
  r->x = 1;
  r->y = 2;
  return (void *) r;
}

int main(int argc, char *argv[]) {
  pthread_t p;
  myret_t *m;

  myarg_t args;
  args.a = 10;
  args.b = 20;
  pthread_create(&p, NULL, mythread, &args);
  pthread_join(p, (void **) &m);
  printf("returned %d %d\n", m->x, m->y);
  free(m); // free the allocated memory
  return 0;
}
```

</td>
<td>

```python
import threading

# Equvalente a struct en C
class MyArg:
    def __init__(self, a, b):
        self.a = a
        self.b = b

class MyRet:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

def mythread(arg, result_container):
    print(arg.a, arg.b)
    result = MyRet(x=1, y=2)
    # join() no retorna un valor: el hilo
    # deposita su resultado en un objeto
    # mutable compartido
    result_container.append(result)

if __name__ == "__main__":
  myarg_t = MyArg(10, 20)
  result_container = []

  p = threading.Thread(target=mythread,
          args=(myarg_t, result_container))
  p.start()
  p.join()

  m = result_container[0]
  print(f"main: result = {m.x}, {m.y}")
```

</td>
</tr>
</table>

| Mismo concepto | Diferencia de API o de lenguaje |
|---|---|
| Los argumentos se agrupan en una estructura (`myarg_t` / `MyArg`) y el resultado en otra (`myret_t` / `MyRet`) | En C la agrupación es obligatoria (un solo `void *`); en Python es opcional, porque `args` acepta varios valores |
| El resultado tiene que sobrevivir al hilo que lo creó | En C se logra con `malloc` (heap) y `main` debe liberarlo con `free`; en Python todo objeto vive mientras alguien lo referencie, y el recolector de basura lo libera |
| `main` espera con `join` antes de leer el resultado | `pthread_join` entrega el valor de retorno; `Thread.join()` no, por eso se usa `result_container` |

(Se omitieron los `#include`, el comentario de compilación, los comentarios de `pthread_join` y un `printf` comentado de `t2.c`; en `t2.py` se resumió el comentario sobre `join()` y se omitieron los comentarios de sección.)

### `t3` — Paso de un valor simple

<table>
<tr><th>C — <code>t3.c</code></th><th>Python — <code>t3.py</code></th></tr>
<tr>
<td>

```c
void *mythread(void *arg) {
    // mismo tamaño que void *
    long long int value = (long long int) arg;
    printf("%lld\n", value);
    return (void *) (value + 1);
}

int main(int argc, char *argv[]) {
    pthread_t p;
    // pthread_join escribe aqui un void * (8 bytes)
    long long int rvalue;
    pthread_create(&p, NULL, mythread, (void *) 100);
    pthread_join(p, (void **) &rvalue);
    printf("returned %lld\n", rvalue);
    return 0;
}
```

</td>
<td>

```python
import threading

def mythread(value, result_container):
    print(value)
    # join() no retorna un valor: el hilo
    # deposita su resultado en un objeto
    # mutable compartido, igual que en t2.py
    result_container.append(value + 1)

if __name__ == "__main__":
  result_container = []

  # el numero va directamente en args
  p = threading.Thread(target=mythread,
          args=(100, result_container))
  p.start()
  p.join()

  rvalue = result_container[0]
  print(f"returned {rvalue}")
```

</td>
</tr>
</table>

| Mismo concepto | Diferencia de API o de lenguaje |
|---|---|
| Al hilo se le pasa un número suelto, sin empaquetarlo en una estructura, y devuelve otro número | En C el número viaja **disfrazado de puntero** dentro del `void *`; en Python va tal cual en la tupla `args` |
| La salida es la misma: `100` / `returned 101` | En C la variable que recibe el retorno debe medir lo mismo que un puntero (8 bytes en 64 bits): la versión de las diapositivas, con `int`, corrompe memoria vecina. En Python no hay punteros ni tamaños de tipo que coordinar, así que ese error **no se puede cometer** |
| `main` espera con `join` antes de usar el resultado | `pthread_join` entrega el valor; `Thread.join()` no, así que `t3.py` reutiliza el contenedor compartido de `t2.py` |

Este par muestra bien la idea del anexo: el concepto (pasar un dato simple a un hilo y recibir otro) es idéntico, pero una dificultad que en C es real —el tamaño de los tipos en la frontera de la API— desaparece en Python, porque su API se construyó sobre otro modelo de datos.

(En `t3.c` se omitieron los `#include`, el comentario de compilación y la versión de las diapositivas que va comentada al inicio; en `t3.py` se resumieron los comentarios sobre `join()` y sobre `args`, y se omitieron los comentarios de sección.)

---

## 3. Dos diferencias de comportamiento que conviene conocer

### Qué pasa si `main` no hace `join`

Se probó una variante de `t0` en la que cada hilo espera 1 segundo antes de imprimir y `main` **no** hace `join`. Resultados en la máquina de prueba (ver [entorno de ejecución](../entorno_ejecucion.md)):

| Versión | Salida |
|---|---|
| C | `main: begin` / `main: end` — los hilos nunca imprimen: al retornar de `main` el proceso termina y se los lleva |
| Python | `main: begin` / `main: end` / `A` / `B` — el intérprete espera a los hilos antes de terminar |
| Python con `daemon=True` | `main: begin` / `main: end` — los hilos *daemon* mueren con el programa, como en C |

El concepto es el mismo (si nadie espera a un hilo, su trabajo puede no completarse), pero la **política por defecto** es opuesta. Es un buen ejemplo de por qué hay que leer la documentación de la API de cada lenguaje.

### Cómo obtener el valor de retorno de un hilo en Python

`t2.py` usa una lista compartida porque es la traducción más directa de `t2.c`. Python ofrece además una API de más alto nivel, `concurrent.futures`, en la que el valor de retorno sí viaja de vuelta, como con `pthread_join`:

```python
from concurrent.futures import ThreadPoolExecutor

def mythread(a, b):
    print(a, b)
    return (1, 2)

with ThreadPoolExecutor() as ex:
    f = ex.submit(mythread, 10, 20)   # crea e inicia (como pthread_create)
    x, y = f.result()                 # espera y recibe el retorno (como pthread_join)
print(f"returned {x} {y}")
```

Salida: `10 20` / `returned 1 2`.

### Una advertencia sobre el paralelismo en Python

La documentación oficial del módulo `threading` lo dice explícitamente: en CPython, por el GIL, *"solo un hilo puede ejecutar código Python a la vez"*; para aprovechar varios núcleos recomienda `multiprocessing` o `concurrent.futures.ProcessPoolExecutor`, y aclara que los hilos siguen siendo el modelo adecuado para ejecutar **varias tareas de I/O** simultáneamente. En otras palabras: en Python, los hilos dan **concurrencia** (superponer cómputo con esperas de red o disco), pero un cálculo de CPU escrito en Python puro **no** se acelera con más hilos. La suma de vectores lo muestra con números: su [versión en Python con `threading`](../sum_vectores/python/README.md) es **más lenta** que la serial (speedup ≈ 0.6–0.8) y usa ~100 % de CPU con 8 hilos, mientras que la versión en C llega a ≈ 4.4 con ~450 % de CPU (ver la [comparación](../sum_vectores/README.md#comparación-c-vs-python)).

Para paralelismo de CPU en Python hay varias rutas: procesos (`multiprocessing`, `ProcessPoolExecutor`), bibliotecas que hacen el trabajo fuera del intérprete y liberan el GIL (como NumPy), y desde Python 3.14 `InterpreterPoolExecutor`, que ejecuta cada tarea en un subintérprete con su propio GIL. Además existe una variante de CPython **sin GIL** (*free-threaded*): fue experimental en Python 3.13 y desde Python 3.14 está soportada oficialmente (PEP 779), pero **no** es la instalación por defecto (su ejecutable suele llamarse `python3.14t`). En esa variante los hilos de Python sí corren en paralelo; `sys._is_gil_enabled()` indica si el GIL está activo en el proceso actual.

El concepto (dividir el trabajo entre núcleos) es el mismo en todos los casos; la herramienta adecuada depende del lenguaje y de su implementación.

### Ver lo que no es atómico: `objdump` en C, `dis` en Python

En la guía principal, `objdump` muestra que `counter = counter + 1` en C son tres instrucciones de máquina. Python tiene una herramienta equivalente para sus **bytecodes**, el módulo `dis`. Con Python 3.8 (la versión de la máquina de prueba), el incremento de `t1.py` se ve así:

```python
import dis
class Counter:
    def __init__(self):
        self.value = 0
counter = Counter()
def incrementa():
    counter.value += 1
dis.dis(incrementa)
```

```
  7           0 LOAD_GLOBAL              0 (counter)
              2 DUP_TOP
              4 LOAD_ATTR                1 (value)
              6 LOAD_CONST               1 (1)
              8 INPLACE_ADD
             10 ROT_TWO
             12 STORE_ATTR               1 (value)
             14 LOAD_CONST               0 (None)
             16 RETURN_VALUE
```

| Paso | C (`objdump`, x86-64) | Python (`dis`, 3.8) |
|---|---|---|
| Leer el valor compartido | `mov 0x2dd1(%rip),%eax` | `LOAD_ATTR (value)` |
| Sumar 1 | `add $0x1,%eax` | `INPLACE_ADD` |
| Escribir el resultado | `mov %eax,0x2dc8(%rip)` | `STORE_ATTR (value)` |

Es el mismo patrón leer–modificar–escribir en dos niveles distintos: instrucciones de la CPU en C y bytecodes del intérprete en Python. Si el cambio de hilo ocurre entre la lectura y la escritura, se pierde una actualización en ambos casos. Los nombres de los bytecodes cambian entre versiones de Python (por ejemplo, desde 3.11 la suma aparece como `BINARY_OP`), pero el patrón se mantiene.

---

## 4. Preguntas

1. Completa la tabla de la sección 1 con una tercera columna para otro lenguaje que conozcas (Java, C++, Go, Rust, ...): ¿cómo se crea un hilo, cómo se espera a que termine y cómo se protege una sección crítica?
2. Comenta los `join` en `t0.c` y en `t0.py`, y agrega un `sleep(1)` (`time.sleep(1)` en Python) al inicio de `mythread`. Compara las salidas y explícalas con la sección 3. Luego crea los hilos de `t0.py` con `daemon=True`: ¿qué cambia?
3. Reescribe `t2.py` usando `ThreadPoolExecutor`, como en la sección 3. ¿Qué parte de `t2.py` desaparece? ¿Cuál de las dos versiones de Python se parece más a `t2.c`?
4. Cambia el argumento a `5000000000` en `t3.c` (la versión corregida), en la versión de las diapositivas y en `t3.py`. ¿Cuál de las tres imprime el valor correcto? ¿Qué tuviste que cuidar en C que en Python no?
5. `t1.py` no puede imprimir la dirección de `i`. ¿Cómo podrías mostrar en Python que cada hilo tiene su propia `i`? (Pista: imprime `i` junto con el nombre del hilo en algunas iteraciones.)
6. Corrige `t1.py` con `threading.Lock()` y `with lock:` (pregunta 6 de `t1` en la guía principal) y compárala con la versión en C con `pthread_mutex_lock`. ¿Qué hace por ti el `with` que en C debes hacer a mano?
7. Ejecuta el fragmento de `dis` de la sección 3 con tu versión de Python (`python3 --version`). ¿Qué bytecodes ves? ¿Siguen formando el patrón leer–modificar–escribir? Ubica en la salida dónde tendría que ocurrir el cambio de hilo para que se pierda una actualización.
8. **Experimento abierto.** `sys.getswitchinterval()` indica cada cuánto (en segundos; 0.005 por defecto) el intérprete pide al hilo actual que ceda el turno, un parámetro parecido al `-i` del simulador `x86.py`. Agrega `sys.setswitchinterval(...)` al inicio de `t1.py` con valores como `0.0001` y `0.05`, y ejecuta varias veces con `1000000`. ¿Cambia la cantidad de actualizaciones perdidas? En la máquina de prueba no se observó un efecto claro (el resultado variaba tanto como sin cambiar el intervalo): ¿por qué podría ser? ¿Qué otros factores deciden cuándo cambia de hilo un programa real, que el simulador no tiene?

---

## Referencias

- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*. Cap. 26 (https://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf) y cap. 27 (https://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf).
- Código de referencia en C: https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-api
- Python, módulo `threading`: https://docs.python.org/3/library/threading.html
- Python, módulo `concurrent.futures`: https://docs.python.org/3/library/concurrent.futures.html
- Python, glosario: *global interpreter lock*: https://docs.python.org/3/glossary.html#term-global-interpreter-lock
- Python, módulo `dis` (desensamblador de bytecode): https://docs.python.org/3/library/dis.html
- Python, *Python support for free threading* (CPython sin GIL): https://docs.python.org/3/howto/free-threading-python.html
- PEP 703, *Making the Global Interpreter Lock Optional in CPython*: https://peps.python.org/pep-0703/ — y PEP 779, que la declara soportada en 3.14: https://peps.python.org/pep-0779/
- Real Python, *An Intro to Threading in Python* y *Speed Up Your Python Program With Concurrency* (citados también en las diapositivas): https://realpython.com/intro-to-python-threading/ y https://realpython.com/python-concurrency/
- Slatkin, B. *Effective Python*, ítem "Use `Lock` to Prevent Data Races in Threads" (el GIL no evita las race conditions en el código del programa).
- `pthreads(7)`, `pthread_create(3)`, `pthread_join(3)`, `pthread_mutex_lock(3)` — páginas de manual de Linux.

> [!note]
> **Nota sobre IA:** este anexo fue elaborado con asistencia de un modelo de inteligencia artificial. Los fragmentos de código reproducen los archivos de esta carpeta (con las omisiones indicadas bajo cada tabla), y las salidas de la sección 3 se obtuvieron ejecutando los programas en la máquina de prueba.
