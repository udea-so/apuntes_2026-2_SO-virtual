# Clase 02 - Conceptos de Sistemas Operativos

Este repositorio contiene el código de ejemplo utilizado en la Clase 02 de Sistemas Operativos.

> [!WARNING]
> Antes de ejecutar este ejemplo revise la sección de **Details** del siguiente [link](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/intro) y siga las recomendaciones para la compilación aqui dadas.

## Contenido

- `Makefile.mk`: Archivo `Makefile` para compilar los programas.
- `cpu.c`: Programa que simula una carga de CPU.
- `threads.c`: Programa que demuestra el uso de hilos.
- `io.c`: Programa que realiza operaciones de entrada/salida (E/S) en un archivo.
- `mem.c`: Programa que simula el uso de memoria.

## Cómo compilar y ejecutar

### Compilar todos los programas

Para compilar todos los programas, simplemente ejecuta `make` en la terminal:

```bash
make
```

Esto creará los ejecutables `cpu`, `threads`, `io` y `mem`.

### Compilar programas individualmente

Puedes compilar programas específicos utilizando el nombre del objetivo en el `Makefile`:

```bash
make cpu
make threads
make io
make mem
```

### Limpiar ejecutables

Para eliminar todos los ejecutables compilados, usa:

```bash
make clean
```

### Ejecutar programas

Una vez compilados, puedes ejecutar los programas de la siguiente manera:

#### `cpu`

Este programa toma un argumento de cadena y lo imprime repetidamente cada 5 segundos.

```bash
./cpu "One"
```

Puedes ejecutar múltiples instancias en segundo plano:

```bash
./cpu "One" & ./cpu "Two" & ./cpu "Three" & ./cpu "Four" &
```

Para detener todas las instancias de `cpu`:

```bash
pkill cpu
```

#### `mem`

Este programa crea una variable usando la llamada `malloc` y la inicializa con el argumento pasado al programa cuando se llama. El programa despliega la direccion de memoria de la variable e incrementa el valor de esta cada segundo.

```bash
./mem 1
```

Puedes ejecutar múltiples instancias en segundo plano:

```bash
./mem 1 & ./mem 100 &
```

Para detener todas las instancias de `mem`:

```bash
pkill mem
```

#### `threads`

Este programa demuestra el uso de hilos para incrementar un contador. Toma un número como argumento, que representa la cantidad de incrementos por hilo.

```bash
./threads 1000
```

Puedes probar con un número mayor para observar la concurrencia:

```bash
./threads 100000
```

#### `io`

Este programa crea un archivo llamado `/tmp/file` y escribe una cadena en él.

```bash
./io
```

Después de ejecutarlo, puedes verificar el contenido del archivo:

```bash
cat /tmp/file
```