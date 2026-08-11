

# Resumen General
En esta sección, vamos a cubrir temas que nos ayudarán a responder las siguientes preguntas:
* ¿Qué es un proceso?
* ¿Qué es la API de procesos y sus llamadas relacionadas?
* ¿Cuáles son los diferentes estados de un proceso?
* ¿Qué es un Bloque de Control de Proceso y cómo se relaciona con la Lista de Procesos?

# Introducción

* Una de las abstracciones más básicas de un sistema operativo es un **proceso**.
* Un **programa** puede considerarse como un conjunto de instrucciones esperando a ser ejecutadas. Una vez que el SO (Sistema Operativo) ejecuta el programa, este se convierte en un **proceso**.
* Al usar un computador, puede parecer que está ejecutando un proceso para cada aplicación al mismo tiempo. No es necesario preocuparnos por cuántos procesos puede manejar el computador, todo simplemente funciona como se espera.
* El sistema operativo *parece* utilizar la **virtualización** para crear varias CPUs virtuales. Sin embargo, el computador *solo tiene una CPU que ejecuta un único proceso a la vez*.
* Una CPU moderna utiliza el tiempo compartido (**time-sharing**), lo que permite ejecutar tus aplicaciones de forma concurrente.
* La **virtualización** de la CPU requiere hardware de bajo nivel e inteligencia de alto nivel para realizarse correctamente.
  * Los **mecanismos** son protocolos o técnicas de bajo nivel que completan las tareas requeridas.
  * Las **políticas** son algoritmos que deciden cómo manejar las numerosas solicitudes y los recursos limitados.
    * Por ejemplo, la **política de planificación** (**scheduling policy**) a menudo decidirá qué programa ejecutar y cómo hacerlo.
  
## ¿Que es un proceso?

Un proceso es, en esencia, un programa que se está ejecutando. Para definirlo completamente, se describen los componentes con los que interactúa durante su ejecución. El concepto clave para entender un proceso es su **estado en la máquina**, que es toda la información que el programa puede leer o modificar en un momento dado.

### ¿Cuales son las partes requeridas para ejecutar un programa?

Para que un programa se ejecute, necesita interactuar con varias partes de la máquina:
* **Memoria**: Aquí se almacenan los datos y las instrucciones que el programa lee y escribe. La porción específica de memoria que un proceso tiene permitido usar se conoce como su espacio de direcciones.
* **Registros**: Son componentes cruciales del estado de la máquina. Su función es obtener, decodificar y ejecutar (fetch - decode - execute) instrucciones. Algunos registros notables incluyen:
* **Contador de Programa (PC)**: Almacena la dirección de memoria de la siguiente instrucción que se debe ejecutar.
* **Puntero de Pila (stack pointer) y Puntero de Marco (frame pointer)**: Se usan para administrar la "pila" (stack), que organiza los parámetros de funciones, las variables locales y las direcciones de retorno.
* **Almacenamiento Persistente**: Se refiere a los dispositivos (como discos duros o SSD) donde se guarda información a largo plazo, por ejemplo, los archivos que un proceso tiene abiertos.


### Creación de un proceso

Las computadoras guardan los **programas** en un disco en un formato ejecutable. Antes de que el SO pueda ejecutar el programa, primero debe realizar varias tareas de preparación:
1. Leer los bytes del archivo del programa desde el disco.
2. Cargar esos bytes en la memoria, la cual se convierte en el espacio de direcciones del proceso.
3. Asignar memoria para la pila de ejecución (run-time stack) del programa. Esta pila se usará para variables locales, parámetros de funciones y direcciones de retorno.
4. Preparar el proceso para solicitudes de E/S (Entrada/Salida), como leer desde el teclado o escribir en la pantalla.

Una vez hecho todo lo anterior, el sistema operativo puede finalmente iniciar la ejecución del programa en su **punto de entrada**, la función `main()`. El control de la CPU se transfiere al nuevo proceso y comienza la ejecución.



<p align="center">
  <img src="fromDisk.gif" alt="Creacion de un proceso" width="400">
</p>

### Estados de un proceso

Una vez que un proceso es creado, puede encontrarse en uno de tres estados principales:
* **En ejecución (Running)**: El proceso está actualmente usando la CPU para ejecutar sus instrucciones.
* **Listo (Ready)**: El proceso está preparado para ejecutarse, pero está en una cola esperando a que el sistema operativo le asigne tiempo en la CPU.
* **Bloqueado (Blocked)**: El proceso no puede continuar porque está esperando que ocurra un evento externo, como la finalización de una operación de E/S (Entrada/Salida).

El sistema operativo gestiona constantemente las transiciones entre estos estados. Cuando un proceso pasa de listo a en ejecución, se dice que ha sido planificado (**scheduled**). Por el contrario, un proceso que pasa de en ejecución a listo ha sido desplanificado (**descheduled**), usualmente para darle la oportunidad a otro proceso.

Una causa común de bloqueo son las operaciones de E/S. Por ejemplo, si un proceso necesita leer un archivo del disco, se bloquea hasta que los datos estén disponibles. Una vez que la operación de E/S se completa, el proceso vuelve al estado listo.

<p align="center">
  <img src="processStates.gif" alt="Texto descriptivo de la imagen" width="400">
</p>

### Explorando los estados de un proceso

A continuación se simula la ejecución de uno o más procesos en una CPU con el fin de visualizar cómo cambia el estado del proceso. Los procesos simulados pueden tener uno de cuatro estados:
* **RUNNING**: el proceso se está ejecutando en la CPU
* **READY**: el proceso está listo para la CPU, pero aún no se está ejecutando
* **WAITING**: el proceso espera E/S antes de ejecutarse
* **DONE**: el proceso ha completado todas sus tareas

## Ejemplo 1

La primera simulación ejecuta un solo proceso (PID: 0) que tiene 5 instrucciones y no utiliza E/S. Debería ver que cada instrucción tiene el estado EN EJECUCIÓN porque el proceso no compite con otro proceso por la CPU.

```
clear && python process_state.py 1
```

La salia es:

```
Time    PID0            CPU     IO
----    ----            ---     --
1       RUN:CPU         1
2       RUN:CPU         1
3       RUN:CPU         1
4       RUN:CPU         1
5       RUN:CPU         1
```

### Ejemplo 2

Ahora añadamos dos procesos. Cada uno tiene cinco instrucciones y no hay operaciones de E/S (Entrada/Salida).

Lo que observarás es que el primer proceso (PID: 0) ejecuta sus cinco instrucciones primero. Durante este tiempo, el segundo proceso (PID: 1) se encuentra en el estado LISTO (READY), y solo cuando el primer proceso termina, el SO (Sistema Operativo) ejecuta el segundo.

```
clear && python process_state.py 2
```

Donde:

```
Time    PID0            PID1            CPU     IO
----    ----            ----            ---     --
1       RUN:CPU         READY           1
2       RUN:CPU         READY           1
3       RUN:CPU         READY           1
4       RUN:CPU         READY           1
5       RUN:CPU         READY           1
6       DONE            RUN:CPU         1
7       DONE            RUN:CPU         1
8       DONE            RUN:CPU         1
9       DONE            RUN:CPU         1
10      DONE            RUN:CPU         1
```

### Ejemplo 3

Analicemos un ejemplo más, esta vez utilizando operaciones de E/S (Entrada/Salida).


El escenario es el siguiente: un proceso necesita completar 3 instrucciones. Cada una de estas instrucciones requiere una operación de E/S, y cada operación de E/S tarda 5 unidades de tiempo en completarse.

¿Cuál esperarías que sea el resultado?

```
clear && python process_state.py 2
```

Donde:

```
Time    PID0            CPU     IO
----    ----            ---     --
1       RUN:IO          1
2       WAITING                 1
3       WAITING                 1
4       WAITING                 1
5       WAITING                 1
6       WAITING                 1
7       RUN:IO DONE     1
8       RUN:IO          1
9       WAITING                 1
10      WAITING                 1
11      WAITING                 1
12      WAITING                 1
13      WAITING                 1
14      RUN:IO DONE     1
15      RUN:IO          1
16      WAITING                 1
17      WAITING                 1
18      WAITING                 1
19      WAITING                 1
20      WAITING                 1
21      RUN:IO DONE     1
```

Click the next button to see some metrics about the execution of the process.    

```
clear && python process_state.py 4
```


```
Time    PID0            CPU     IO
----    ----            ---     --
1       RUN:IO          1
2       WAITING                 1
3       WAITING                 1
4       WAITING                 1
5       WAITING                 1
6       WAITING                 1
7       RUN:IO DONE     1
8       RUN:IO          1
9       WAITING                 1
10      WAITING                 1
11      WAITING                 1
12      WAITING                 1
13      WAITING                 1
14      RUN:IO DONE     1
15      RUN:IO          1
16      WAITING                 1
17      WAITING                 1
18      WAITING                 1
19      WAITING                 1
20      WAITING                 1
21      RUN:IO DONE     1

Stats
-----
Total time: 21
CPU busy: 6 (25.57%)
IO busy: 15 (71.43%)
```

Finalmente, tengamos dos procesos, con uno de ellos realizando una operación de E/S. Tal como antes, el PID: 0 se ejecuta primero, pero esta vez la primera instrucción usa E/S, por lo que el estado cambia de EN EJECUCIÓN a EN ESPERA. Mientras el primer proceso está esperando, el PID: 1 ejecuta sus tres instrucciones. Cuando la instrucción de E/S finaliza, el PID: 0 termina sus dos últimas instrucciones.

```
clear && python process_state.py 5
```

```
Time    PID0            PID1            CPU     IO
----    ----            ----            ---     --
1       RUN:IO          READY           1
2       WAITING         RUN:CPU         1       1
3       WAITING         RUN:CPU         1       1
4       WAITING         RUN:CPU         1       1
5       WAITING         DONE                    1
6       WAITING         DONE                    1
7       RUN:IO DONE     DONE            1
8       RUN:CPU         DONE            1
9       RUN:CPU         DONE            1

Stats
-----
Total time: 9
CPU busy: 7 (77.78%)
IO busy: 5 (55.56%)
```

