# Compilación y ejecución de los ejemplos (contadores)

Breve guía para compilar y ejecutar los ejemplos en este directorio. Supuestos: sistema Linux con gcc y pthreads instalados.

## Estructura relevante
- **[include/](./include/)**: encabezados comunes (common.h, common_threads.h)
- **[counter_without_locks/](./counter_without_locks/)**: ejemplo sin sincronización (counter.c, counter_without_locks.c, test_counter_cli.c)
- **sloppy_counter/ y [precise_counter/](./precise_counter/)**: otros ejemplos (cada uno con su .c y .h)

## Instrucciones generales

1. Abrir una terminal y colocarse en el directorio del ejemplo, por ejemplo:
   cd clase_16/code/counters/counter_without_locks

2. Compilar el ejemplo principal (sin argumentos):
   
   ```
   gcc -Wall -I../include counter.c counter_without_locks.c -lpthread -o bad_counter
   ```

   Ejecutar:

   ```
   ./bad_counter
   ```

   Este ejecutable usa valores definidos en el código (NUM_THREADS, CNT_END) y muestra el tiempo y el valor final del contador.

3. Compilar el cliente de prueba con argumentos (test_counter_cli.c):
   
   ```
   gcc -Wall -I../include test_counter_cli.c counter.c -lpthread -o counter_test
   ```

   Ejecutar (ejemplo con 8 hilos y 1_000_000 incrementos por hilo):
   
   ```
   ./counter_test 8 1000000
   ```

   Nota: `test_counter_cli` permite variar número de hilos y conteos por hilo desde la línea de comandos.

## Compilar otros ejemplos (genérico)

- Desde el directorio `code/counters`, para cada subdirectorio con código fuente:
  
  ```
  cd <subdir>
  gcc -Wall -I../include *.c -lpthread -o <nombre_ejecutable>
  ```

  Reemplazar `<subdir>` por `sloppy_counter` o `precise_counter` y `<nombre_ejecutable>` por el nombre deseado (por ejemplo `sloppy_counter`).

## Limpieza

- Eliminar ejecutables y objetos:
  
  ```
  rm -f *.o bad_counter counter_test sloppy_counter precise_counter
  ```

> [!note]
> **Nota sobre IA**: Este contenido fue elaborado y estructurado con la asistencia de un modelo de inteligencia artificial. Puede contener errores.
