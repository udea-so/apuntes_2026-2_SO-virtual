# Semaforo binario (Implementacion para sincronizar)

## Resumen

Ejemplo que demuestra el uso de un semáforo como variable de condición para sincronizar el orden de ejecución entre hilos  

### Archivos

- `sem_as_CV.c`      — ejemplo que demuestra ordenamiento/sincronización (puede usar semáforos o CV según el enfoque).
- `common.h`        — utilidades comunes (timers, macros, etc.).
- `common_threads.h`— envoltorios para operaciones con hilos y sincronización.
- `Makefile`        — reglas de compilación y limpieza.

## Compilación (recomendado)

1. Abrir terminal y situarse en el directorio:
   
   ```
   cd clase_19/code/diapositivas/ordering_sem
   ```

2. Compilar con Make:
   
   ```
   make
   ```

   - El nombre del ejecutable depende del Makefile (revisar su contenido). Si la regla principal falla, use la compilación manual indicada abajo.

## Compilación (manual)

- Compilar manualmente:
  
  ```
  gcc -Wall -I. sem_as_CV.c -lpthread -o sem_as_CV
  ```

## Ejecución

- Ejecutar el binario generado:
  
  ```
  ./sem_as_CV
  ```


## Limpieza
- Usar Make:
  
  ```
  make clean
  ```

- O eliminar binario manualmente:
  
  ```
  rm -f sem_as_CV
  ```

> [!Note]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality.
