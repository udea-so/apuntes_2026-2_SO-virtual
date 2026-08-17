# Tarea (Simulación) — Planificación de CPU

Este programa, [scheduler.py](scheduler.py), permite ver el rendimiento de diferentes planificadores según métricas como el tiempo de respuesta, el tiempo de procesamiento y el tiempo total de espera. Consulte el archivo [README](https://github.com/remzi-arpacidusseau/ostep-homework/blob/master/cpu-sched/README.md) para obtener más información.

1. Calcule el response time y el turnaround time al ejecutar tres trabajos de longitud 200 empleando los planificadores SJF y FIFO.
2. Repita el ejercicio anterior, pero con trabajos de longitudes distintas: 100, 200 y 300.
3. Repita nuevamente el ejercicio, incluyendo esta vez el planificador RR con un time-slice de 1.
4. ¿Para qué tipo de cargas de trabajo (workloads) SJF entrega los mismos turnaround times que FIFO?
5. ¿Para qué tipo de cargas de trabajo y longitudes de quantum SJF entrega los mismos response times que RR?
6. ¿Qué sucede con el response time en SJF a medida que aumenta la longitud de los trabajos? ¿Puede emplear el simulador para demostrar dicha tendencia?
7. ¿Qué sucede con el response time en RR a medida que aumenta la longitud del quantum? ¿Puede formular una ecuación que exprese el response time en el peor caso, dado *N* trabajos?

