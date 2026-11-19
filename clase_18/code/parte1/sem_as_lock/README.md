# Implementación de un Mutex utilizando Semáforos

Este proyecto demuestra cómo construir un mecanismo de **Exclusión Mutua (Mutex)** utilizando primitivas de **Semáforos POSIX**.

El objetivo principal es resolver el problema de la **Condición de Carrera (Race Condition)** que ocurre cuando dos hilos intentan modificar una variable compartida (`counter`) simultáneamente sin sincronización.

## Estructura del Proyecto

El código está modularizado para separar la lógica del programa de la implementación de la herramienta de sincronización:

* **`binary.c`**: Programa principal. Crea dos hilos que incrementan un contador compartido dentro de una sección crítica protegida.
* **`my_lock.c` / `my_lock.h`**: Librería personalizada. Define la estructura `lock_t` e implementa las operaciones `init`, `acquire` y `release` usando semáforos.
* **`common_threads.h`**: Contiene "wrappers" (envoltorios) para las llamadas al sistema (como `Pthread_create` o `Sem_wait`), manejando automáticamente los errores con `assert`.
* **`Makefile`**: Script de compilación automatizada.

## Cómo Compilar

Este proyecto incluye un `Makefile` para facilitar la compilación modular.

1.  Abre tu terminal en la carpeta del proyecto.
2.  Ejecuta el siguiente comando:

```bash
make
```
Esto generará el ejecutable llamado `binary`.

**Nota**: Si deseas limpiar los archivos generados (`.o` y ejecutables) para recompilar desde cero, ejecuta:


```Bash
make clean
```

## Cómo Probar el Funcionamiento

Una vez compilado, ejecuta el programa:

```Bash
./binary
```

Resultado Esperado
El programa lanza dos hilos. Cada uno incrementa el contador `10.000.000` de veces. Si el Mutex funciona correctamente, el resultado final debe ser exacto:


```
result: 20000000 (should be 20000000)
```
Si la implementación del lock fuera incorrecta o no existiera, el resultado sería un número menor y aleatorio (ej. `14592301`) debido a las condiciones de carrera.

## Preguntas de Reflexión para el Estudiante

Para asegurarte de que has comprendido los conceptos clave, intenta responder estas preguntas antes de mirar el código fuente o después de ejecutarlo:

1. **Sobre la Inicialización**: En `my_lock.c`, la función init inicializa el semáforo con el valor `1`.
   * ¿Qué pasaría si lo inicializaras en `0`?
   * ¿Podrían los hilos entrar a la sección crítica? ¿Por qué?
2. **Sobre la Atomicidad**: ¿Por qué es necesario usar `acquire` antes de `counter++`?
   * ¿Qué sucede a nivel de instrucciones de máquina (ensamblador) cuando se ejecuta `counter++` que lo hace peligroso en entornos concurrentes?
3. **Semáforos vs. Spinlocks**: En acquire, usamos `Sem_wait`. Si el semáforo está ocupado, el hilo se bloquea (se duerme).
   * ¿Cuál es la ventaja de este enfoque frente a un bucle `while` (espera activa) que compruebe constantemente una variable?
4. **Orden de Operaciones**: Si en `binary.c` intercambiamos el orden y ponemos `release(&mutex)` antes de `counter++`:
   * ¿Seguiría estando protegida la variable compartida?
   * ¿Qué principio de la exclusión mutua se estaría violando?

## Resultados de Aprendizaje

Al finalizar esta práctica, ejecutar el código y responder las preguntas de reflexión, el estudiante será capaz de:

1.  **Implementar Exclusión Mutua con Semáforos:**
    Comprender cómo un mecanismo generalizado (semáforo) puede restringirse para actuar como un candado binario (mutex), inicializando su valor en **1** y usándolo estrictamente en pares `wait`/`post`.

2.  **Identificar y Proteger Secciones Críticas:**
    Reconocer las líneas de código donde ocurre el acceso concurrente a recursos compartidos (la variable `counter`) y aplicar la protección necesaria para evitar la corrupción de datos.

3.  **Analizar el Impacto de las Condiciones de Carrera:**
    Evidenciar experimentalmente cómo la falta de sincronización destruye el determinismo de un programa (obteniendo resultados aleatorios) y cómo el uso correcto del *lock* restaura la consistencia (obteniendo siempre `20000000`).

4.  **Aplicar Modularidad en C:**
    Entender la importancia de desacoplar la lógica de negocio (`binary.c`) de la implementación de bajo nivel (`my_lock.c`), utilizando archivos de cabecera (`.h`) para definir interfaces claras.

5.  **Manejar APIs POSIX:**
    Familiarizarse con las llamadas al sistema estándar para hilos y semáforos en Linux (`pthread_create`, `sem_init`, `sem_wait`, `sem_post`), así como el uso de *wrappers* para un manejo robusto de errores.

> [!Note]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality.