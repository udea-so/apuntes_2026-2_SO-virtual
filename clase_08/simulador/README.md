# Tarea (Simulación) - Dynamic Relocation

El programa [`relocation.py`](relocation.py) permite observar cómo se realizan las traducciones de direcciones en un sistema con registros base y límite (base and bounds). Se recomienda consultar el [README](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-mechanism) para más detalles.

## Preguntas

1. Ejecutar el programa con las semillas (seeds) 1, 2 y 3, y determinar si cada dirección virtual generada por el proceso se encuentra dentro o fuera de los límites (in bounds / out of bounds). Si se encuentra dentro de los límites, calcular la traducción correspondiente.

2. Ejecutar con los siguientes parámetros: `-s 0 -n 10`. ¿A qué valor se debe fijar `-l` (el registro de límite/bounds) para asegurar que todas las direcciones virtuales generadas estén dentro de los límites?

3. Ejecutar con los siguientes parámetros: `-s 1 -n 10 -l 100`. ¿Cuál es el valor máximo al que se puede fijar `base` de modo que el espacio de direcciones siga cabiendo por completo dentro de la memoria física?

4. Repetir algunos de los ejercicios anteriores, pero con espacios de direcciones más grandes (`-a`) y memorias físicas más grandes (`-p`).

5. ¿Qué fracción de las direcciones virtuales generadas aleatoriamente resulta válida, en función del valor del registro de límite (bounds)? Se debe generar una gráfica ejecutando el programa con distintas semillas aleatorias, con valores de límite (limit) que vayan desde 0 hasta el tamaño máximo del espacio de direcciones.