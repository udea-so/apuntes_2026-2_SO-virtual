# Tarea (Simulación) - Segmentación

El programa [`segmentation.py`](segmentation.py) permite observar cómo se realizan las traducciones de direcciones en un sistema con segmentación. Se recomienda consultar el [README](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-segmentation) para más detalles.

## Preguntas

1. Utilice primero un espacio de direcciones pequeño para traducir algunas direcciones. Ejecute el programa con los parámetros `-a 128 -p 512 -b 0 -l 20 -B 512 -L 20`, probando con las semillas (seeds) 0, 1 y 2, y traduzca las direcciones generadas en cada caso.

2. Teniendo en cuenta el espacio de direcciones construido en la pregunta anterior (con los mismos parámetros `-a 128 -p 512 -b 0 -l 20 -B 512 -L 20`): ¿cuál es la dirección virtual legal más alta en el segmento 0? ¿Cuál es la dirección virtual legal más baja en el segmento 1? ¿Cuáles son las direcciones ilegales más baja y más alta de todo el espacio de direcciones? Finalmente, ¿cómo ejecutaría `segmentation.py` con la bandera `-A` para comprobar si sus respuestas son correctas?

3. Suponga que tiene un espacio de direcciones de 16 bytes dentro de una memoria física de 128 bytes (`-a 16 -p 128`). ¿Qué valores de base y límite (`--b0`, `--l0`, `--b1`, `--l1`) debe configurar para que, al traducir la secuencia de direcciones `-A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15`, el simulador entregue los siguientes resultados en orden: válida, válida, violación, ..., violación, válida, válida?

4. Suponga que desea generar un problema en el que aproximadamente el 90% de las direcciones virtuales generadas aleatoriamente resulten válidas (es decir, sin violación de segmentación). ¿Cómo debe configurar el simulador para lograrlo? ¿Qué parámetros son los más importantes para conseguir ese resultado?

5. ¿Es posible ejecutar el simulador de modo que ninguna dirección virtual generada sea válida? ¿Cómo lo lograría?