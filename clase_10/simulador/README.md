# Tarea (Simulación) - Paginación

El programa [`paging-linear-translate.py`](paging-linear-translate.py) permite verificar si comprende cómo funciona la traducción simple de direcciones virtuales a físicas con tablas de página lineales. Se recomienda consultar el [README](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-paging) para más detalles.

## Preguntas

1. Antes de realizar cualquier traducción, empleemos el simulador para estudiar cómo cambia de tamaño la tabla de página lineal según diferentes parámetros. Calcule el tamaño de las tablas de página lineales a medida que cambian los distintos parámetros. A continuación se sugieren algunas entradas; empleando la bandera `-v`, usted puede ver cuántas entradas de la tabla de página están llenas. Primero, para entender cómo cambia el tamaño de la tabla de página lineal a medida que crece el espacio de direcciones, ejecute con las siguientes banderas:
   - `-P 1k -a 1m -p 512m -v -n 0`
   - `-P 1k -a 2m -p 512m -v -n 0`
   - `-P 1k -a 4m -p 512m -v -n 0`

2. Luego, entendamos cómo cambia el tamaño de la tabla de página lineal a medida que crece el tamaño de página. Antes de ejecutar cualquiera de estos, intente pensar en las tendencias esperadas. ¿Cómo debería cambiar el tamaño de la tabla de página a medida que crece el espacio de direcciones? ¿A medida que crece el tamaño de página? ¿Por qué no emplear páginas grandes en general?
   - `-P 1k -a 1m -p 512m -v -n 0`
   - `-P 2k -a 1m -p 512m -v -n 0`
   - `-P 4k -a 1m -p 512m -v -n 0`

3. Ahora realicemos algunas traducciones. Comience con algunos ejemplos pequeños, y cambie el número de páginas que se asignan al espacio de direcciones mediante la bandera `-u`. ¿Qué sucede a medida que aumenta el porcentaje de páginas asignadas en cada espacio de direcciones?
   - `-P 1k -a 16k -p 32k -v -u 0`
   - `-P 1k -a 16k -p 32k -v -u 25`
   - `-P 1k -a 16k -p 32k -v -u 50`
   - `-P 1k -a 16k -p 32k -v -u 75`
   - `-P 1k -a 16k -p 32k -v -u 100`

4. Ahora probemos algunas semillas aleatorias distintas, y algunos parámetros de espacio de direcciones diferentes (y a veces bastante disparatados), para tener variedad. ¿Cuáles de estas combinaciones de parámetros son irreales? ¿Por qué?
   - `-P 8 -a 32 -p 1024 -v -s`
   - `-P 8k -a 32k -p 1m -v -s 2`
   - `-P 1m -a 256m -p 512m -v -s 3`

5. Emplee el programa para probar algunos otros problemas. ¿Puede encontrar los límites en los que el programa deja de funcionar? Por ejemplo, ¿qué sucede si el tamaño del espacio de direcciones es mayor que la memoria física?