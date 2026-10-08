# Ejemplos clase 14


## Motivación

> [!tip]
> "¿Qué sucedió con el prejuicio que existía hace dos años en el sentido de que la programación paralela es difícil? Resulta que lo que era difícil, casi imposible, es tomar un programa ordinario e imaginar automáticamente la forma de utilizar la computación paralela en dicho programa. En lugar de ello, hay que empezar de cero con el problema, considerando que tenemos la posibilidad de cálculo paralelo, y reescribir por completo el programa con una nueva [comprensión de] lo que hay dentro de la máquina.  
> 
> No es posible utilizar efectivamente los viejos programas. Deben reescribirse. Esto es una gran desventaja para la mayoría de las aplicaciones industriales y ha encontrado una resistencia considerable. Pero los grandes programas pertenecen normalmente a científicos u otros programadores inteligentes, no funcionarios, a quienes les gusta la ciencia de la computación y están dispuestos a empezar de cero y reescribir el programa si pueden hacerlo más eficaz.  
> 
> De modo que lo que va a suceder es que los programas difíciles, los muy grandes, serán los primeros en ser reprogramados por expertos a la nueva manera, y luego, gradualmente, todo el mundo tendrá que aceptarlo; más y más programas se programarán de esa manera, y los programadores simplemente tendrán que aprender a hacerlo"

<div align="center">
  <img src="The_pleasure_of_finding_things_out.jpg" alt="El placer de descubrir" width="70%">
</div>

## Ruta de lectura

El material de esta clase sigue los capítulos 26 (*Concurrency: An Introduction*) y 27 (*Interlude: Thread API*) de OSTEP. Cada guía se puede leer por separado, pero este es el orden sugerido: cada paso usa lo visto en el anterior.

| Paso | Material | OSTEP | Qué vas a observar |
|---|---|---|---|
| 0 | [Entorno de ejecución](ejemplos/entorno_ejecucion.md) | — | En qué máquina y entorno se obtuvieron las salidas de las guías, y por qué tus resultados pueden ser distintos |
| 1 | [`t0`: creación de hilos](ejemplos/api-thread/README.md#ejemplo-1--creación-de-hilos-y-orden-no-determinista-t0) | §26.2 | Que el orden en que corren los hilos no lo decide el programa sino el scheduler, y que `pthread_join` es lo único que garantiza un orden |
| 2 | [`t1`: datos compartidos](ejemplos/api-thread/README.md#ejemplo-2--datos-compartidos-y-race-condition-t1) | §26.3 y §26.4 | Que dos hilos que incrementan el mismo contador pierden actualizaciones, y en el ensamblador de tu propio binario, por qué: `counter = counter + 1` son tres instrucciones |
| 3 | [Simulación con `x86.py`](simulacion/README.md) | §26.4 y §26.6 (tarea del cap. 26) | La misma race condition, pero eligiendo tú dónde caen las interrupciones; dónde está exactamente la sección crítica; y un hilo que espera a otro revisando la memoria |
| 4 | [`t2`: argumentos y retorno](ejemplos/api-thread/README.md#ejemplo-3--argumentos-y-valores-de-retorno-vía-structs-t2) | §27.1 y §27.2 | Cómo pasar varios datos a un hilo y recibir su resultado con structs, y por qué el resultado debe vivir en el heap y no en el stack del hilo |
| 5 | [`t3`: valor simple](ejemplos/api-thread/README.md#ejemplo-4--paso-de-un-valor-simple-t3) | §27.2 (Fig. 27.3) | Cómo pasar un número directamente en el `void *`, y qué se rompe en 64 bits si los tipos no tienen el mismo tamaño |
| 6 | Pregunta 5 de [`t1`](ejemplos/api-thread/README.md#preguntas-para-probar-cambios-1) | §27.3 | Cómo la API de locks (`pthread_mutex_lock`/`unlock`) elimina la race condition del paso 2, y cuánto cuesta |
| 7 | [Suma de vectores](ejemplos/sum_vectores/README.md) | §26.1 | Para qué sirven los hilos: dividir el trabajo entre núcleos y medir el speedup, que depende de si el límite es la CPU o la memoria |

La suma de vectores corresponde a la primera sección del capítulo 26 (*Why Use Threads?*), pero va al final porque usa lo de los pasos 1 a 4: crea varios hilos, les pasa un `struct` de argumentos y espera a todos con `pthread_join`.

## Codigo

Ejemplos basicos sobre el API thread.
1. Ejemplos basicos de programación con hilos [[link]](ejemplos/api-thread/) en C y python. Incluye un anexo que compara las versiones en C y en Python [[link]](ejemplos/api-thread/anexo_c_python.md).
2. Ejemplo de suma de vectores (solución serial y paralela) [[link]](ejemplos/sum_vectores/)
3. Entorno de ejecución de los ejemplos (WSL, máquinas virtuales, Docker y Linux nativo) [[link]](ejemplos/entorno_ejecucion.md)
4. Tarea de simulación de condiciones de carrera con `x86.py` [[link]](simulacion/)

## Recursos online

El canal de youtube Core Dumped explica mucho mejor los conceptos de lo que los explicamos en clase. La idea es complementar la densa explicacion teorica empleando estos videos.
1. Concurrency is not what you think [[link]](https://www.youtube.com/watch?v=3X93PnKRNUo)
2. Why Are Threads Needed On Single Core Processors" [[link]](https://www.youtube.com/watch?v=M9HHWFp84f0)
3. Threads On Multicore Systems [[link]](https://www.youtube.com/watch?v=5sw9XJokAqw)


## Referencias

1. Concurrency: An Introduction [[link]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
2. Interlude: Thread API [[link]](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf). Código de referencia: ostep-code/threads-api [[link]](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-api)
3. CSCI 5451: Introduction to parallel computing [[link]](https://www-users.cse.umn.edu/~saad/csci5451/). Ver: An introduction to the Posix Thread API ([link](https://www-users.cse.umn.edu/~saad/csci5451/FILES/LecN6_R.pdf))
4. CSCE 790: Parallel Programming Models for Multicore and Manycore Processors, Fall 2017 [[link]](https://passlab.github.io/CSCE790/). Ver: Lecture 08: Programming with PThreads:	PThreads basics, Mutual	Exclusion and Locks, and Examples ([link](https://passlab.github.io/CSCE790/notes/lecture08_PThreads.pdf))
5. Bootlin [[link]](https://bootlin.com/training/kernel/) Using the POSIX API Threads, real­time and IPC([link](https://hhoegl.informatik.hs-augsburg.de/elinux/rt/posix-api.pdf))
6. POSIX thread (pthread) libraries [[link]](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)
7. CSE 333: Systems programming [[link]](https://courses.cs.washington.edu/courses/cse333/25su/)
8. CSE351: The Hardware/Software Interface [[link]](https://courses.cs.washington.edu/courses/cse351/)
9. PThreads Primer [[link]](https://www.cl72.org/100winProg/pthread-primer.pdf)
10. https://www.cs.washington.edu/academics/courses/
 



