![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Estructuras de datos basadas en locks — Parte 1: Contador sin locks

Guía de ejemplos autónoma para la Clase 16 de Sistemas Operativos.  
Esta parte cubre el primer problema de la cadena: qué ocurre cuando múltiples
hilos comparten una variable sin ningún mecanismo de protección.

---

## Requisitos

| Herramienta | Versión mínima | Verificación |
|---|---|---|
| GCC | 7.0 | `gcc --version` |
| pthreads | (incluida en glibc) | `ldconfig -p \| grep libpthread` |
| Python + Jupyter | 3.8 / 6.0 | `jupyter --version` (opcional, para el notebook) |

El código usa únicamente POSIX threads (`-lpthread`) y la librería estándar de
C. No requiere dependencias externas.

---

## Conceptos previos

### La operación `value++` no es atómica

Cuando un programa en C escribe `value++`, el compilador genera al menos tres
instrucciones de máquina separadas:

1. **LOAD** — leer el valor actual de memoria a un registro del procesador.
2. **ADD** — sumar 1 al valor en el registro.
3. **STORE** — escribir el resultado de vuelta a memoria.

Mientras un hilo ejecuta estas tres instrucciones, el sistema operativo puede
interrumpirlo en cualquier punto (al final de su *quantum* de tiempo) y darle
la CPU a otro hilo. Si ese otro hilo también está leyendo y escribiendo la
misma variable, ambos pueden terminar con la misma lectura inicial y uno de los
dos incrementos se pierde. A este fenómeno se le llama **condición de carrera**
(*race condition*).

### Por qué el resultado siempre es menor o igual al esperado

Suponga dos hilos, T1 y T2, con `value = 5`:

```
T1: LOAD  → registro_T1 = 5
             [el SO interrumpe T1 aquí]
T2: LOAD  → registro_T2 = 5
T2: ADD   → registro_T2 = 6
T2: STORE → value = 6
             [el SO retoma T1]
T1: ADD   → registro_T1 = 6
T1: STORE → value = 6   ← el incremento de T2 se sobrescribió
```

Ambos hilos hicieron un incremento, pero `value` pasó de 5 a 6 en lugar de 7.
Un incremento se perdió. Este patrón se repite miles de veces durante la
ejecución, de ahí que el resultado final sea notablemente menor al esperado.

### Thread-safety y región crítica

Una función o estructura de datos es **thread-safe** si produce resultados
correctos cuando es accedida simultáneamente por múltiples hilos. La parte del
código que accede a un recurso compartido y debe ejecutarse sin interrupción se
llama **región crítica** (*critical section*). En este ejemplo, el cuerpo de
`increment()` —concretamente `c->value++`— es la región crítica desprotegida.

### El comportamiento no es determinista

Una condición de carrera **no produce siempre el mismo error**. El resultado
depende de cuándo exactamente el planificador interrumpe cada hilo, lo cual
cambia de una ejecución a la siguiente. En una máquina con un solo núcleo, la
carrera solo ocurre si el planificador interrumpe un hilo exactamente entre el
LOAD y el STORE; en una máquina con múltiples núcleos, dos hilos pueden
ejecutar esas instrucciones literalmente al mismo tiempo. Por esa razón el
mismo código puede dar el resultado correcto en una corrida y uno incorrecto en
la siguiente, o comportarse de forma distinta según el hardware.

### Escenario real

Este problema ocurre en cualquier sistema donde múltiples procesos actualizan
un contador compartido sin coordinación: el sistema de votos de una red social
(likes, retweets), el inventario de unidades disponibles en una plataforma de
e-commerce durante un evento de alta demanda, o los contadores de acceso
concurrente a un recurso en un servidor web. En todos esos casos, incrementos
perdidos significan datos incorrectos — votos que no se registran, stock que
queda negativo, o métricas de uso que no cuadran.

---

## Organización de los archivos

| Archivo | Rol | Concepto que ilustra |
|---|---|---|
| `counter.h` | Define `counter_t` y declara las funciones | Estructura del contador sin protección |
| `counter.c` | Implementa `init`, `increment`, `decrement`, `get` | Las operaciones no son thread-safe |
| `counter_without_locks.c` | Ejecutable con parámetros fijos (8 hilos, 1 M iteraciones) | Condición de carrera observable |
| `test_counter_cli.c` | Ejecutable con parámetros por línea de comandos | Exploración sistemática del problema |
| `common.h` | Utilidad `GetTime()` de OSTEP | Medición de tiempo de pared |
| `common_threads.h` | Macros wrapper de pthreads con assert | Manejo seguro de errores de threading |
| `pruebas_contador_sin_locks.ipynb` | Notebook Python con datos experimentales | Análisis del error en función del número de hilos |

---

## Ejemplo 1 — `bad_counter`: parámetros fijos

### Contexto teórico

`counter_without_locks.c` lanza exactamente 8 hilos. Cada hilo llama a
`increment()` un millón de veces sobre un único `counter_t` compartido. La
función `increment` contiene solo `c->value++` sin ningún lock. Al terminar, el
programa imprime el valor esperado (8 × 1,000,000 = 8,000,000) y el valor
real obtenido. La diferencia entre ambos es la magnitud de los incrementos
perdidos por condiciones de carrera.

### Compilación

Ejecutar desde el directorio raíz del ejemplo:

```bash
gcc -Wall counter.c counter_without_locks.c -lpthread -o bad_counter
```

### Ejecución

```bash
./bad_counter
```

### Salida esperada

El valor real varía entre ejecuciones. La siguiente salida es representativa;
el número exacto en tu máquina será diferente:

```
-> Tiempo gastado: 0.016229 seg
-> El contador debe quedar en: 8000000
-> El valor real del contador es: 2113671
```

En máquinas con un solo núcleo el resultado puede coincidir con el esperado en
algunas corridas — ver la nota a continuación.

### Por qué este output demuestra el concepto

El valor obtenido (~2.1 M) es aproximadamente el 26 % del valor esperado
(8 M). Eso significa que cerca del 74 % de los incrementos se perdieron por
sobrescritura. El tiempo (~0.016 s) es bajo porque no hay ningún mecanismo de
sincronización que introduzca espera — el programa es rápido pero incorrecto.

> **Nota sobre observabilidad y número de núcleos**
>
> La condición de carrera es **no determinista**. En un sistema con un solo
> núcleo de CPU, el planificador solo puede intercalar hilos en los puntos de
> interrupción del quantum; si los 8 hilos no son interrumpidos exactamente
> durante el intervalo LOAD–STORE, el resultado puede ser correcto. En una
> máquina con múltiples núcleos los hilos se ejecutan físicamente en paralelo
> y la carrera es mucho más frecuente. No hay ningún mecanismo artificial
> (`sched_yield()` ni `sleep()`) en este código para forzar el error: la
> condición de carrera se produce por la ejecución concurrente natural
> gestionada por el SO. Si en tu máquina el resultado siempre es correcto,
> prueba aumentar el número de hilos o el número de iteraciones con
> `test_counter_cli` (ver Ejemplo 2).

---

## Ejemplo 2 — `counter_test`: exploración por línea de comandos

### Contexto teórico

`test_counter_cli.c` expone exactamente la misma lógica que `bad_counter` pero
acepta dos argumentos: número de hilos y número de iteraciones por hilo. Esto
permite explorar cómo escala el error sin recompilar. Pasa el conteo por
iteración al hilo usando `intptr_t` — una forma segura de empaquetar un entero
en un `void *` sin asumir que ambos tipos tienen el mismo tamaño.

### Compilación

```bash
gcc -Wall test_counter_cli.c counter.c -lpthread -o counter_test
```

### Ejecución

**Prueba con un solo hilo (sin carrera posible):**

```bash
./counter_test 1 1000000
```

Salida esperada:

```
Iniciando prueba con 1 hilos, 1000000 incrementos cada uno.

-> Tiempo gastado: 0.005684 seg
-> El contador debe quedar en: 1000000
-> El valor real del contador es: 1000000
```

Con un solo hilo no hay concurrencia — el resultado es siempre correcto.

**Prueba con múltiples hilos (carrera observable):**

```bash
./counter_test 8 1000000
```

Salida esperada (el valor real varía entre ejecuciones):

```
Iniciando prueba con 8 hilos, 1000000 incrementos cada uno.

-> Tiempo gastado: 0.015528 seg
-> El contador debe quedar en: 8000000
-> El valor real del contador es: 2160000
```

### Experimentos recomendados

**1. Escalar el número de hilos manteniendo el trabajo total constante:**

```bash
for t in 1 2 4 8 16; do
    ./counter_test $t 1000000
done
```

Permite observar cómo el error aumenta a medida que hay más hilos compitiendo.

**2. Observar la variabilidad entre corridas:**

```bash
for i in $(seq 1 5); do
    printf "Run %d: " $i
    ./counter_test 8 1000000 | grep "valor real"
done
```

Demuestra que el resultado cambia entre ejecuciones — evidencia directa de que
el bug es no determinista y dependiente del scheduling del SO.

**3. Buscar el umbral donde aparece el error:**

```bash
for n in 1000 10000 100000 500000 1000000; do
    ./counter_test 4 $n
done
```

Con muy pocas iteraciones la carrera puede no materializarse; con muchas es
casi inevitable.

### Por qué este output demuestra el concepto

La comparación entre la corrida con 1 hilo (resultado exacto) y la corrida con
8 hilos (resultado con pérdidas) aísla la causa: no es el código de
`increment()` lo que está mal en términos de lógica secuencial — es que la
operación no es segura cuando múltiples hilos la ejecutan simultáneamente. El
mismo `value++` que funciona perfectamente solo falla en concurrencia.

---

## Análisis de datos — Notebook

El archivo `pruebas_contador_sin_locks.ipynb` contiene los datos experimentales
reales (corridas en la máquina del curso) con gráficas de tiempo de ejecución
y porcentaje de error por número de hilos. Para ejecutarlo:

```bash
jupyter notebook pruebas_contador_sin_locks.ipynb
```

Los datos del notebook muestran que con 16 hilos el error llega al 92.6 %
(se pierde casi todo el trabajo). Nótese que los valores obtenidos del notebook
difieren ligeramente de los de las diapositivas — por ejemplo, 4 hilos produce
`1,039,790` en el notebook y `1,033,970` en las slides. Ambas son corridas
distintas del mismo código: la variación entre ejecuciones es en sí misma parte
del fenómeno que se está demostrando.

---

## Limpieza

```bash
rm -f bad_counter counter_test
```

---

## Conclusiones de este grupo

Este ejemplo recorre el primer eslabón de la cadena:

| Mecanismo | Qué resuelve | Qué limitación introduce |
|---|---|---|
| Contador sin lock | Máxima velocidad, código simple | Los resultados son incorrectos con más de un hilo |
| *(siguiente: lock global)* | Resultados correctos con N hilos | El lock serializa todos los accesos — escala pobremente |
| *(siguiente: sloppy counter)* | Escala bien en múltiples núcleos | El valor global es aproximado, no exacto |

El problema demostrado aquí — un recurso compartido modificado sin exclusión
mutua — es el caso base que motiva todos los mecanismos de sincronización del
resto de la clase.

---

## Referencias

- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy
  Pieces*. Capítulo 26 (*Concurrency: An Introduction*) y Capítulo 29
  (*Lock-based Concurrent Data Structures*, sección "Concurrent Counters").
  Disponible en: <https://pages.cs.wisc.edu/~remzi/OSTEP/>

- Documentación de POSIX Threads:
  `man 7 pthreads`, `man pthread_create`, `man pthread_join`

- Código de referencia del libro (capítulo 29):
  <https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf>

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Tenga en cuenta que puede contener errores.

> [!warning]
> **Aclaración**: Como todo código (especialmente los ejemplos didácticos diseñados pueden fallar (o de hecho fallan)), el contenido debe ser revisado críticamente y puede contener errores.