# Implementación de una Variable de Condición (Signaling) utilizando Semáforos

Este proyecto demuestra cómo construir un mecanismo de **Señalización (Signaling)** —similar a una Variable de Condición simplificada— utilizando primitivas de **Semáforos POSIX**.

El objetivo principal es resolver el problema de la **Coordinación de Hilos (Ordering)**, asegurando que un proceso (el padre) espere pasivamente a que otro proceso (el hijo) complete su tarea antes de continuar, evitando el uso ineficiente de ciclos de CPU (espera activa).

## Estructura del Proyecto

El código está modularizado para separar la lógica del programa de la implementación de la herramienta de sincronización:

* **`sem_as_CV.c`**: Programa principal. Crea un hilo "hijo" y utiliza la estructura `my_cv` para que el hilo principal ("padre") espere hasta que el hijo termine.
* **`my_cv.c` / `my_cv.h`**: Librería personalizada. Define la estructura `my_cv` e implementa las operaciones `init`, `wait` (esperar señal) y `signal` (enviar señal) usando semáforos.
* **`common_threads.h`**: Contiene "wrappers" (envoltorios) para las llamadas al sistema (como `Pthread_create`, `Sem_wait` o `Sem_post`), manejando automáticamente los errores con `assert`.
* **`Makefile`**: Script de compilación automatizada que vincula los módulos objetos.

## Cómo Compilar

Este proyecto incluye un `Makefile` para facilitar la compilación modular.

1.  Abre tu terminal en la carpeta del proyecto.
2.  Ejecuta el siguiente comando:

```bash
make
````

Esto compilará los módulos y generará el ejecutable llamado `sem_as_CV`.

**Nota**: Si deseas limpiar los archivos generados (`.o` y ejecutables) para recompilar desde cero, ejecuta:

```bash
make clean
```

## Cómo Probar el Funcionamiento

Una vez compilado, ejecuta el programa:

```bash
./sem_as_CV
```

### Resultado Esperado

El programa debe mostrar una secuencia estricta donde el padre espera a que el hijo imprima su mensaje. El orden de salida **siempre** debe respetar la finalización del hijo antes de que el padre termine:

```text
parent: begin
child
parent: end
```

*(Nota: Puede haber un ligero retardo de 2 segundos antes de "child" debido al `sleep` simulado, pero "parent: end" jamás debe aparecer antes que "child").*

## Preguntas de Reflexión para el Estudiante

Para asegurarte de que has comprendido los conceptos clave, intenta responder estas preguntas antes de mirar el código fuente o después de ejecutarlo:

1.  **Sobre la Inicialización**: En `my_cv.c`, la función `init` inicializa el semáforo con el valor `0`.
      * ¿Qué pasaría si lo inicializaras en `1` (como en un Mutex)?
      * ¿El padre esperaría realmente al hijo o continuaría inmediatamente? ¿Por qué?
2.  **Sobre el paso de Argumentos**: En `sem_as_CV.c` refactorizado, pasamos `&cond` como argumento al hilo hijo en lugar de usar una variable global.
      * ¿Qué ventaja ofrece esto si quisiéramos coordinar múltiples pares de hilos padre-hijo independientes en el mismo programa?
3.  **Semáforos vs. Variables de Condición Puras**: Una Variable de Condición estándar (`pthread_cond_wait`) siempre requiere un Mutex asociado.
      * En nuestra implementación simplificada con semáforos, no usamos un Mutex explícito en el `wait`. ¿Por qué funciona en este caso simple de señalización (Signaling) pero podría fallar en problemas más complejos como el Productor-Consumidor?
4.  **Manejo de Recursos**: Si el hilo hijo finalizara abruptamente (crash) antes de llamar a `my_cv_signal`:
      * ¿Qué le sucedería al hilo padre?
      * ¿Cómo se llama a esta situación en programación concurrente?

## Resultados de Aprendizaje

Al finalizar esta práctica, ejecutar el código y responder las preguntas de reflexión, el estudiante será capaz de:

1.  **Implementar Señalización con Semáforos:**
    Comprender cómo inicializar un semáforo en **0** permite usarlo como una barrera o señal, donde `sem_wait` bloquea hasta que ocurre un evento (`sem_post`).

2.  **Evitar la Espera Activa (Busy Waiting):**
    Diferenciar entre comprobar una variable en un bucle `while` (que consume CPU inútilmente) y bloquear el proceso usando primitivas del SO (`Sem_wait`), permitiendo que el planificador asigne la CPU a otros procesos.

3.  **Modularizar Primitivas de Sincronización:**
    Aplicar buenas prácticas de ingeniería de software encapsulando la lógica de sincronización en una estructura propia (`my_cv`) y funciones asociadas, ocultando la complejidad de los semáforos subyacentes al programa principal.

4.  **Manejo de Memoria y Argumentos en Hilos:**
    Practicar el paso de punteros a estructuras (`struct my_cv *`) a través de `pthread_create`, evitando el uso de variables globales y mejorando la reusabilidad del código.

5.  **Depuración de Flujo de Ejecución:**
    Verificar visualmente (mediante `printf`) y lógicamente cómo las primitivas de sincronización alteran y garantizan el orden determinista de ejecución entre hilos asíncronos.

> [!NOTE]
> **AI Disclosure:** This document was created with the assistance of Artificial Intelligence language models. The content has been reviewed, edited, and validated by a human author to ensure accuracy and quality.

