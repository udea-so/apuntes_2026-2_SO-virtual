# Tarea (Simulación) - Multi-Level Feedback Queue

Este programa, [`mlfq.py`](mlfq.py), permite observar cómo se comporta el planificador MLFQ presentado en el capitulo 8 del libro **Scheduling: The Multi-Level Feedback Queue** ([link](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-mlfq.pdf)). Consulte el archivo [README](https://github.com/remzi-arpacidusseau/ostep-homework/blob/master/cpu-sched-mlfq/README.md), (escrito por el autor) para más detalles.

## Preguntas

1. Ejecute algunos problemas generados aleatoriamente con solo dos trabajos y dos colas; calcule la traza de ejecución de MLFQ para cada uno. Facilítese la vida limitando la longitud de cada trabajo y desactivando las I/O.
2. ¿Cómo ejecutaría el planificador para reproducir cada uno de los ejemplos del capítulo?
3. ¿Cómo configuraría los parámetros del planificador para que se comporte igual que un planificador round-robin?
4. Diseñe una carga de trabajo con dos trabajos y unos parámetros del planificador de modo que uno de los trabajos aproveche las antiguas Reglas 4a y 4b (activadas con la bandera `-S`) para "engañar" al planificador y obtener el 99% de la CPU durante un intervalo de tiempo particular.
5. Dado un sistema con una longitud de quantum de 10 ms en su cola de mayor prioridad, ¿con qué frecuencia tendría que elevar (boost) los trabajos de vuelta al nivel de mayor prioridad (con la bandera `-B`) para garantizar que un único trabajo de larga duración (y potencialmente en riesgo de starvation) obtenga al menos el 5% de la CPU?
6. Una pregunta que surge en la planificación es a cuál extremo de una cola se debe agregar un trabajo que acaba de terminar una I/O; la bandera `-I` modifica este comportamiento en este simulador de planificación. Experimente con algunas cargas de trabajo y observe si puede notar el efecto de esta bandera.
