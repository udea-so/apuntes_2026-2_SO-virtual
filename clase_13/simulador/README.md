# Tarea (Simulación) - Políticas de reemplazo de páginas

El programa [`paging-policy.py`](paging-policy.py) permite experimentar con diferentes políticas de reemplazo de páginas (`FIFO`, `LRU`, `OPT`, `UNOPT`, `RAND`, `CLOCK`). Se recomienda consultar el [README](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-beyondphys-policy) para más detalles.

## Preguntas

1. Genere direcciones aleatorias con los siguientes argumentos. Cambie la política de `FIFO`, a `LRU`, a `OPT` (bandera `-p`). Calcule si cada acceso en dichas trazas de direcciones es un acierto (*hit*) o un fallo (*miss*).
   - `-s 0 -n 10`
   - `-s 1 -n 10`
   - `-s 2 -n 10`

2. Para un caché de tamaño 5 (`-C 5`), genere flujos de referencias de direcciones del peor caso para cada una de las siguientes políticas: `FIFO`, `LRU` y `MRU` (los flujos de referencias del peor caso son los que causan la mayor cantidad posible de fallos). Para los flujos de referencias del peor caso, ¿cuánto más grande debe ser el caché para mejorar el rendimiento drásticamente y acercarse a `OPT`?

3. Genere una traza aleatoria (es decir, use Python o Perl y escriba un script que imprima direcciones aleatorias, las cuales luego puede pasarle al simulador). ¿Cómo esperaría que se desempeñen las diferentes políticas con una traza así?

4. Ahora genere una traza con algo de localidad. ¿Cómo puede generar una traza así? ¿Cómo se desempeña `LRU` con ella? ¿Qué tanto mejor que `RAND` es `LRU`? ¿Cómo le va a `CLOCK`? ¿Y a `CLOCK` con diferentes números de bits de reloj (bandera `-b`)?

5. Use un programa como `valgrind` para instrumentar una aplicación real y generar un flujo de referencias a páginas virtuales. Por ejemplo, ejecutar `valgrind --tool=lackey --trace-mem=yes ls` producirá una traza casi completa de cada referencia a instrucciones y datos que hace el programa `ls`. Para que esto sea útil para el simulador anterior, primero tendrá que transformar cada referencia a memoria virtual en una referencia a número de página virtual (lo cual se hace enmascarando el desplazamiento y desplazando hacia abajo los bits resultantes). ¿Qué tamaño de caché se necesita para que la traza de su aplicación satisfaga una gran fracción de las solicitudes? Grafique su conjunto de trabajo (*working set*) a medida que aumenta el tamaño del caché.
