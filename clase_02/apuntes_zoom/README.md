![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Introducción a los Sistemas Operativos: Virtualización de Recursos

## Objetivos de Aprendizaje

* **Definir**: Qué es un sistema operativo y su doble función como intermediario que administra recursos y como proveedor de abstracciones limpias para los programadores.
* **Explicar**: El ciclo de instrucción (*fetch-decode-execute*) y cómo el hardware y el software se organizan en capas (usuarios, aplicaciones, sistema operativo, hardware).
* **Distinguir**: Los distintos tipos de sistemas de cómputo (dedicado, compartido, portátil, embebido) según su compromiso entre usabilidad y uso de recursos.
* **Demostrar**: Mediante los programas `cpu.c`, `mem.c`, `threads.c` e `io.c`, cómo el sistema operativo virtualiza CPU, memoria, concurrencia y persistencia.
* **Enumerar**: Los objetivos de diseño de un sistema operativo (abstracción, desempeño, protección, confiabilidad).

## ¿Qué Hace un Sistema Operativo?

### ¿Cómo se Ejecuta un Programa?

Para ejecutar un programa, la CPU repite un ciclo de cuatro pasos:

1. **Fetch**: obtiene la instrucción de memoria.
2. **Decode**: averigua la instrucción a ejecutar.
3. **Exec**: realiza la operación indicada.
4. Se salta a la próxima instrucción y se repite el procedimiento.

$$\text{Computador} = \text{Hardware} + \text{Software}$$

Este ciclo (basado en el **modelo de Von Neumann**) se ilustró con un ejemplo en ensamblador MIPS sobre variables `x` y `y` almacenadas en `.data` (`lw`, `li`, `add`, `sw`), donde la CPU va leyendo instrucciones desde memoria (`Instrucciones`) y operando sobre los datos (`Datos`).

### ¿Cómo Facilitar la Ejecución de Varios Programas?

La sensación de que **todas las aplicaciones se ejecutan a la vez** (como un malabarista manejando varias pelotas al tiempo) es, en realidad, producto de que el sistema operativo reparte rápidamente el hardware (CPU, memoria) entre las instrucciones y datos de cada programa.

### Estructura de los Sistemas de Cómputo

Un sistema de cómputo se organiza en capas:

1. **Usuarios**: personas, máquinas u otras computadoras.
2. **Aplicaciones**: programas del sistema y de usuario (compiladores, procesadores de texto, navegadores — `compiler`, `assembler`, `text editor`, `database system`, …).
3. **Sistema Operativo**: controla y coordina el uso del hardware entre varias aplicaciones y usuarios.
4. **Hardware**: CPU, memoria, dispositivos de E/S.

### Clasificación de los Sistemas de Cómputo

La clasificación depende del punto de vista y del compromiso entre **usabilidad** y **utilización de recursos**:

| Tipo | Prioridad |
| --- | --- |
| **Computadora de usuario único** | No importa mucho la utilización de los recursos. |
| **Computadora compartida** (ej. servidor de impresión) | Mantener a todos los usuarios felices. |
| **Sistema dedicado** (ej. mainframe) | Compromiso entre usabilidad y utilización de recursos. |
| **Dispositivo portátil** | Gestionar recursos limitados, optimizar la usabilidad. |
| **Sistemas embebidos** (ej. Smart TV) | Interfaz de usuario muy limitada o inexistente. |

## Definición de Sistema Operativo

Es un **software** que:

1. **Administra** los recursos de un sistema de cómputo (CPU, memoria, disco duro) de forma eficiente y justa (*fair*).
2. **Controla** la ejecución de programas: previene errores y evita el uso inadecuado del sistema de cómputo.

No hay una definición universalmente aceptada; el manuscrito recoge dos referencias:

> **Tanenbaum**: *"An OS is a software that performs two essentially unrelated functions: providing application programmers a clean abstract set of resources instead of the messy hardware ones and managing these hardware resources."*

> **Arpaci-Dusseau**: *"An OS is a body of software that is responsible for making it easy to run programs, allowing programs to share memory, enabling programs to interact with devices, …, as it is in charge of making sure the system operates correctly and efficiently in an easy-to-use manner."*

## Virtualización de Recursos

El sistema operativo actúa como una **máquina virtual**: toma un **recurso físico** y lo transforma en una **forma virtual** de sí mismo, más general, potente y fácil de usar.

| Recurso físico | Forma virtual |
| --- | --- |
| Procesador | Proceso |
| Memoria | Espacio de direccionamiento |
| Disco | Archivo |

Las **llamadas al sistema** (*system calls*) permiten que el usuario le diga al SO qué hacer; el SO expone esta funcionalidad mediante una interfaz (**API**, librería estándar — por ejemplo, POSIX): ejecutar programas, acceder a memoria, acceder a dispositivos, entre cientos de otras llamadas (`fork()`/`exit()`/`wait()`, `open()`/`read()`/`write()`/`close()`, `getpid()`, `pipe()`, `chmod()`, …).

Como **administrador de recursos**, el SO permite:

* La ejecución de múltiples programas → **compartiendo la CPU**.
* Que múltiples programas accedan simultáneamente a sus datos e instrucciones → **compartiendo la memoria**.
* Que múltiples programas accedan a los dispositivos → **compartiendo los discos**.

### Virtualización de la CPU

**La ilusión**: el sistema tiene un gran número de CPUs virtuales — convierte una CPU en "infinitas" CPUs virtuales, permitiendo que múltiples programas se ejecuten "de manera concurrente" (**proceso**, **hilo**, **archivo ejecutable**).

**Ejemplo — `cpu.c`**: un programa que imprime repetidamente una cadena de texto pasada como argumento.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

double getTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

void wait(int howlong) {
    double t = getTime();
    while ((getTime() - t) < (double) howlong)
      ; //wait...
}

int main(int argc, char *argv[])
{
    char *str = argv[1]; //string we passed

    while (1) {
      printf("%s\n", str);
      wait(5);
    }
    return 0;
}
```

Ejecutando un solo proceso:

```
prompt> ./a.out hola
hola
hola
hola
...
```

Y dos procesos a la vez (`./a.out hola & ./a.out mundo`), sobre un único recurso físico (CPU):

```
prompt> ./a.out hola & ./a.out mundo
[1] 1301
mundo
hola
mundo
hola
...
```

> [!Note]
> Para ejecutar estos ejemplos en equipos con Windows se recomendó instalar **WSL** (Windows Subsystem for Linux) y compilar con el `Makefile` provisto (`make`).

### Virtualización de Memoria

La memoria física **es un arreglo de bytes**: un programa la lee (**load**, especificando una dirección) y la escribe (**write**, especificando el dato y la dirección).

**Ejemplo — `mem.c`**: asigna un entero con `malloc`, imprime la dirección apuntada por `p` y la incrementa en un ciclo infinito.

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

double getTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

void wait(int howlong) {
    double t = getTime();
    while ((getTime() - t) < (double) howlong)
      ; //wait...
}

int main(int argc, char *argv[]) {
    int *p;
    p = malloc(sizeof(int));
    assert(p != NULL);
    printf("(%d) addr pointed to by p: %p\n", (int) getpid(), p);
    *p = atoi(argv[1]);
    while (1) {
        Spin(1);
        *p = *p + 1;
        printf("(%d) value of p: %d\n", getpid(), *p);
    }
    return 0;
}
```

Al correr **dos instancias** del mismo programa, ambas imprimen la **misma dirección virtual** (`0x100100080`), pero cada una modifica un contador físico independiente:

```
prompt> ./2-mem & ./2-mem
[1] 1336
(1336) address pointed to by p: 0x100100080
(1337) address pointed to by p: 0x100100080
(1336) p: 0x100100080
(1337) p: 0x100100080
...
```

### Concurrencia

Cuando varios **hilos (*threads*)** de un mismo proceso comparten memoria (código, datos y archivos, pero cada uno con sus propios registros y *stack*), aparecen **problemas de concurrencia**.

**Ejemplo — condición de carrera**: dos hilos ejecutan `deposit(amount)` sobre un `balance` compartido, que en ensamblador se traduce en tres instrucciones (`load`, `add`, `store`):

```c
void deposit(int amount) {
    balance = balance + amount;
}
```
```asm
load R1, balance
add  R1, amount
store R1, balance
```

Si el *Thread 1* (`deposit(10)`) y el *Thread 2* (`deposit(20)`) se entrelazan — por ejemplo, ambos ejecutan `load` y `add` antes de que cualquiera ejecute `store` — el resultado final puede perder una de las dos actualizaciones.

**Ejemplo — `threads.c`**: dos hilos incrementan un `counter` compartido `loops` veces cada uno.

```c
volatile int counter = 0;
int loops;

void *worker(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
      counter++;
    }
    return NULL;
}
```

Con pocas iteraciones el resultado es el esperado (el doble de `loops`), pero al aumentar el número de iteraciones el resultado se vuelve **impredecible**:

```
./threads 10        → Final value : 20
./threads 100       → Final value : 200
./threads 1000      → Final value : 2000
./threads 10000     → Final value : 20000
./threads 100000    → Final value : 156427   (se esperaba 200000)
./threads 100000    → Final value : 163340
./threads 100000    → Final value : 139245
```

### Persistencia

La memoria principal usa una tecnología **volátil**: los datos se pierden si deja de haber alimentación eléctrica. Por eso es necesario un mecanismo de **persistencia**:

* **Hardware**: dispositivos de almacenamiento (discos duros, discos de estado sólido, SD, USB).
* **Software**: el **sistema de archivos**, responsable de almacenar los archivos del usuario de forma confiable y eficiente sobre el disco (organizado como un árbol: `/`, `bin`, `dev`, `home`, `etc`, …).

**Ejemplo — `io.c`**: crea el archivo `/tmp/file` y escribe una cadena de texto en él.

```c
int main(int argc, char *argv[]) {
    int fd = open("/tmp/file", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fd >= 0);
    char buffer[20];
    sprintf(buffer, "hello world\n");
    int rc = write(fd, buffer, strlen(buffer));
    assert(rc == (strlen(buffer)));
    fsync(fd);
    close(fd);
    return 0;
}
```

```
prompt> ./per
prompt> cat file_tmp
hello world
```

## Objetivos de Diseño de los Sistemas Operativos

1. **Construir abstracciones**: hacen que el sistema sea fácil de usar (por ejemplo, el proceso como abstracción de la CPU).
2. **Proporcionar un buen desempeño**: minimizar el sobrecosto (*overhead*) del SO — la virtualización no debe generar un sobrecosto excesivo.
3. **Protección entre aplicaciones**: aislamiento — un mal comportamiento de un proceso no debe afectar a otros procesos ni al sistema.
4. **Confiabilidad**: el SO debería ejecutarse sin problema por un plazo largo de tiempo.
5. **Otros objetivos**: eficiencia energética, seguridad, movilidad.

---

> [!IMPORTANT]
> **Nota de Transparencia:** Este documento fue generado y adaptado mediante el uso de **IA Generativa**, a partir del manuscrito anotado de la clase y el resumen de la sesión de Zoom del 06/08/2026. El contenido ha sido supervisado, validado y refinado por intervención humana para garantizar su precisión técnica y coherencia pedagógica. No obstante, pueden haber errores.
