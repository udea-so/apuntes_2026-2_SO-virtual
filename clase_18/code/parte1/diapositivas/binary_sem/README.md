# Semaforo binario (Implementacion como mutex)

## Resumen

El siguiente ejemplo demuestra el uso de un semáforo binario como mutex para proteger una sección crítica.

### Archivos
- `binary.c`        — código del ejemplo (crea hilos y usa el semáforo).
- `common.h`        — utilidades comunes (timers, impresión, etc.).
- `common_threads.h`— wrappers para operaciones de hilos y sincronización.
- `Makefile`        — reglas para compilar/limpiar.

## Compilación (recomendado)

1. Abrir terminal y situarse en el directorio:
   
   ```
   cd clase_19/code/diapositivas/binary_sem
   ```

2. Compilar con Make:
   
   ```
   make
   ```

   - Ejecutable generado: revisar el Makefile para el nombre exacto (normalmente `binary` o similar).
   - Si Makefile no existe o falla, usar compilación manual (ver abajo).

## Compilación (manual)

- Si quiere compilar sin make, ejecute:
  
  ```
  gcc -Wall -I. binary.c -lpthread -o binary
  ```

## Ejecución

- Ejecutar el binario generado:
  
  ```
  ./binary
  ```

## Limpieza

- Usar Make:
  
  ```
  make clean
  ```

- O eliminar binarios manualmente:
  
  ```
  rm -f binary
  ```

> [!Note]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality.