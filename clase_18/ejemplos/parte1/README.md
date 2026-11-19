# Ejemplos de Sincronización — Clase 19

Este directorio contiene los ejemplos desarrollados para ilustrar el uso de semáforos y otros mecanismos de sincronización en el contexto del curso de **Sistemas Operativos (SO)**.  

Los programas están organizados en subdirectorios según el propósito didáctico de cada conjunto de ejemplos: desde casos simples usados en presentaciones hasta implementaciones más completas de semáforos como *locks* y como *variables de condición*.

El objetivo es permitir al estudiante **ejecutar y observar el comportamiento real** de estos mecanismos, facilitando la comprensión de los conceptos teóricos vistos en clase.

## Estructura general del directorio `code/`

A continuación, se presenta un resumen del contenido y finalidad de cada subdirectorio incluido en `code/`.

---

## 1. `diapositivas/`

Contiene el código utilizado directamente en las diapositivas de clase. Los ejemplos son deliberadamente simples y se centran en demostrar conceptos puntuales de sincronización usando semáforos.

---

### 1.1. `diapositivas/binary_sem/`

Ejemplos que ilustran el uso básico de **semáforos binarios**.

**Finalidad:**
- Mostrar cómo un semáforo binario controla el acceso a una sección crítica.
- Introducir el patrón clásico `wait()` / `signal()`.
- Comparar la ejecución con y sin semáforo para evidenciar condiciones de carrera.

---

### 1.2. `diapositivas/ordering_sem/`

Ejemplos diseñados para demostrar cómo los semáforos pueden usarse para **forzar un orden específico** entre hilos.

**Finalidad:**
- Establecer dependencias de ejecución (“A debe ejecutarse antes de B”).
- Mostrar sincronización que no se relaciona con exclusión mutua.
- Introducir el concepto de *ordering constraints* en concurrencia.

---

## 2. `sem_as_lock/`

Ejemplos donde un semáforo se utiliza para implementar el comportamiento de un **mutex (lock)**.

**Finalidad:**
- Comparar semáforos .vs. locks nativos.
- Demostrar cómo un semáforo inicializado en 1 puede actuar como mecanismo de exclusión mutua.
- Permitir al estudiante observar:
  - Condiciones de carrera sin protección.
  - Corrección al usar un semáforo como lock.
  - Cambios en el comportamiento cuando aumenta el número de hilos.

> Nota: Aunque funcional, usar semáforos como locks **no es el patrón recomendado** cuando existen primitivas específicas para exclusión mutua.

---

## 3. `sem_as_cv/`

Ejemplos donde un semáforo se utiliza para emular el comportamiento de una **variable de condición (condition variable)**.

**Finalidad:**
- Enseñar cómo funcionan las operaciones `wait()` y `signal()` en las variables de condición.
- Mostrar cómo simular “esperar una condición” usando únicamente semáforos.
- Ilustrar casos como productor/consumidor mediante la emulación manual de la semántica de CV.

Estos ejemplos permiten observar:
- El desbloqueo del lock antes de dormir.
- La señalización que despierta exactamente un hilo.
- La complejidad de implementar CVs sin soporte nativo.

---

## ¿Cómo usar estos ejemplos?

1. Compile cada archivo con `gcc` (o el compilador indicado en cada ejemplo).
2. Ejecute los binarios varias veces para observar:
   - Condiciones de carrera.
   - Cambios en el orden de ejecución.
   - Efectos del bloqueo/sincronización.
3. Modifique parámetros como número de hilos o tiempos de espera para analizar el comportamiento.

> [!Note]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality. This document may contain errors.