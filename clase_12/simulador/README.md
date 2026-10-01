
## Tarea (Medición)

Esta tarea le presenta una nueva herramienta, `vmstat`, y cómo puede emplearse para comprender el uso de memoria, CPU y E/S. Lea el [README](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-beyondphys) asociado y examine el código en [`mem.c`](mem.c) antes de continuar con los ejercicios y preguntas que se presentan a continuación.

### Preguntas

1. Primero, abra dos conexiones de terminal separadas a la misma máquina, de modo que pueda ejecutar fácilmente algo en una ventana y en la otra.

   Ahora, en una ventana, ejecute `vmstat 1`, que muestra estadísticas sobre el uso de la máquina cada segundo. Lea la página man, el README asociado y cualquier otra información que necesite para comprender su salida. Deje esta ventana ejecutando `vmstat` durante el resto de los ejercicios que se presentan a continuación.

   Ahora, ejecutaremos el programa `mem.c`, pero con un uso de memoria muy bajo. Esto puede lograrse escribiendo `./mem 1` (que usa solamente 1 MB de memoria). ¿Cómo cambian las estadísticas de uso de CPU al ejecutar `mem`? ¿Tienen sentido los números de la columna de tiempo de usuario (user time)? ¿Cómo cambia esto al ejecutar más de una instancia de `mem` a la vez?

2. Comencemos ahora a observar algunas de las estadísticas de memoria mientras se ejecuta `mem`. Nos enfocaremos en dos columnas: `swpd` (la cantidad de memoria virtual usada) y `free` (la cantidad de memoria inactiva). Ejecute `./mem 1024` (que asigna 1024 MB) y observe cómo cambian estos valores. Luego finalice el programa en ejecución (escribiendo control-c) y observe de nuevo cómo cambian los valores. ¿Qué nota en los valores? En particular, ¿cómo cambia la columna `free` cuando el programa termina? ¿Aumenta la cantidad de memoria libre en la cantidad esperada cuando `mem` termina?

3. A continuación, observaremos las columnas de swap (`si` y `so`), que indican cuánto swapping se está llevando a cabo hacia y desde el disco. Por supuesto, para activarlas, deberá ejecutar `mem` con grandes cantidades de memoria. Primero, examine cuánta memoria libre hay en su sistema Linux (por ejemplo, escribiendo `cat /proc/meminfo`; escriba `man proc` para obtener detalles sobre el sistema de archivos `/proc` y los tipos de información que puede encontrar allí). Una de las primeras entradas de `/proc/meminfo` es la cantidad total de memoria en su sistema. Supongamos que es algo así como 8 GB de memoria; si es así, comience ejecutando `mem 4000` (aproximadamente 4 GB) y observando las columnas de swap in/out. ¿Alguna vez muestran valores distintos de cero? Luego, intente con 5000, 6000, etc. ¿Qué sucede con estos valores cuando el programa entra en el segundo ciclo (y los siguientes), en comparación con el primer ciclo? ¿Cuántos datos (en total) se intercambian hacia dentro y hacia fuera durante el segundo, el tercer y los ciclos subsiguientes? (¿tienen sentido los números?)

4. Realice los mismos experimentos anteriores, pero ahora observe las otras estadísticas (como la utilización de CPU y las estadísticas de E/S de bloques). ¿Cómo cambian cuando `mem` se está ejecutando?

5. Examinemos ahora el desempeño. Elija una entrada para `mem` que quepa cómodamente en la memoria (por ejemplo, 4000 si la cantidad de memoria del sistema es de 8 GB). ¿Cuánto tarda el ciclo 0 (y los ciclos subsiguientes 1, 2, etc.)? Ahora elija un tamaño que esté cómodamente más allá del tamaño de la memoria (por ejemplo, 12000, nuevamente suponiendo 8 GB de memoria). ¿Cuánto tardan los ciclos en este caso? ¿Cómo se comparan los números de ancho de banda? ¿Cuán diferente es el desempeño cuando se realiza swapping constantemente frente


<!--
https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-beyondphys-policy
-->
