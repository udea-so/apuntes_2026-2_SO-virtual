## Tarea (Medición)

Esta tarea le presenta una nueva herramienta, `vmstat`, y cómo puede emplearse para comprender el uso de memoria, CPU y E/S. Lea el [README](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-beyondphys) asociado y examine el código en [`mem.c`](mem.c) antes de continuar con los ejercicios y preguntas que se presentan a continuación.

### Preparación

Requisitos: un sistema Linux con `gcc`, `make` y `vmstat`.

Compile el programa (`EXTRACTO_OSTEP`: es el `Makefile` del repositorio, que ejecuta `gcc -o mem mem.c -Wall -O`):

```sh
make
```

Ejecute `mem` indicando el tamaño del arreglo en MB. El programa se ejecuta indefinidamente y debe detenerse con control-c:

```sh
./mem 1
```

Para todos los ejercicios, trabaje con **dos terminales** conectadas a la misma máquina:

- **Terminal A:** `vmstat 1`, que debe permanecer ejecutándose durante todos los ejercicios.
- **Terminal B:** el programa `mem`.

Para consultar la memoria total de su sistema (necesaria en las preguntas 3, 5 y 6):

```sh
grep MemTotal /proc/meminfo
```

> **Advertencia:** ejecutar `mem` con tamaños cercanos o superiores a la memoria del sistema puede volverlo muy lento o provocar que el sistema operativo finalice procesos. Se recomienda hacerlo en una máquina virtual o en un computador donde no haya trabajo sin guardar.

### Preguntas

#### Pregunta 1

Primero, abra dos conexiones de terminal separadas a la misma máquina, de modo que pueda ejecutar fácilmente algo en una ventana y en la otra.

Ahora, en una ventana, ejecute `vmstat 1`, que muestra estadísticas sobre el uso de la máquina cada segundo. Lea la man page, el README asociado y cualquier otra información que necesite para comprender su salida. Deje esta ventana ejecutando `vmstat` durante el resto de los ejercicios que se presentan a continuación.

Ahora, ejecute el programa `mem.c`, pero con un uso de memoria muy bajo. Esto puede lograrse escribiendo `./mem 1` (que usa solamente 1 MB de memoria). ¿Cómo cambian las estadísticas de uso de CPU al ejecutar `mem`? ¿Tienen sentido los números de la columna de tiempo de usuario (user time)? ¿Cómo cambia esto al ejecutar más de una instancia de `mem` a la vez?

**Cómo ejecutarlo**

Terminal A:

```sh
vmstat 1
```

Terminal B:

```sh
./mem 1
```

Observe el grupo de columnas `cpu` (en particular `us`, `sy` e `id`). Detenga el programa con control-c.

Para ejecutar más de una instancia a la vez, lance varias en segundo plano sin imprimir su salida:

```sh
nproc
for i in 1 2 3; do ./mem 1 > /dev/null & done
```

`nproc` indica cuántas CPU tiene el sistema. Varíe el número de instancias y observe de nuevo las columnas de `cpu`. Para detenerlas todas:

```sh
pkill -x mem
```

#### Pregunta 2

A continuación, observe algunas de las estadísticas de memoria mientras se ejecuta `mem`. Preste atención a dos columnas: `swpd` (la cantidad de memoria virtual usada) y `free` (la cantidad de memoria inactiva). Ejecute `./mem 1024` (que asigna 1024 MB) y observe cómo cambian estos valores. Luego finalice el programa en ejecución (escribiendo control-c) y observe de nuevo cómo cambian los valores. ¿Qué nota en los valores? En particular, ¿cómo cambia la columna `free` cuando el programa termina? ¿Aumenta la cantidad de memoria libre en la cantidad esperada cuando `mem` termina?

**Cómo ejecutarlo**

Terminal A: `vmstat 1` (o `vmstat -S M 1` para mostrar las columnas de memoria en MB).

Terminal B:

```sh
./mem 1024
```

Observe `swpd` y `free` mientras el programa se ejecuta. Luego detenga el programa con control-c y siga observando las mismas columnas.

#### Pregunta 3

A continuación, observe las columnas de swap (`si` y `so`), que indican cuánto swapping se está llevando a cabo hacia y desde el disco. Por supuesto, para activarlas, deberá ejecutar `mem` con grandes cantidades de memoria. Primero, examine cuánta memoria libre hay en su sistema Linux (por ejemplo, escribiendo `cat /proc/meminfo`; escriba `man proc` para obtener detalles sobre el sistema de archivos `/proc` y los tipos de información que puede encontrar allí). Una de las primeras entradas de `/proc/meminfo` es la cantidad total de memoria en su sistema. Suponga que es algo así como 8 GB de memoria; si es así, comience ejecutando `mem 4000` (aproximadamente 4 GB) y observando las columnas de swap in/out. ¿Alguna vez muestran valores distintos de cero? Luego, intente con 5000, 6000, etc. ¿Qué sucede con estos valores cuando el programa entra en el segundo loop (y los siguientes), en comparación con el primer loop? ¿Cuántos datos (en total) se intercambian hacia dentro y hacia fuera durante el segundo, el tercer y los loops subsiguientes? (¿tienen sentido los números?)

**Cómo ejecutarlo**

Examine la memoria del sistema:

```sh
cat /proc/meminfo
man proc
```

Suponiendo 8 GB de memoria, en la Terminal B ejecute, uno a la vez y deteniendo cada uno con control-c antes de lanzar el siguiente:

```sh
./mem 4000
./mem 5000
./mem 6000
```

Continúe con tamaños mayores. Observe las columnas `si` y `so` de `vmstat`, y compare lo que sucede durante el primer loop (`loop 0`) con los loops siguientes en la salida de `mem`.

#### Pregunta 4

Realice los mismos experimentos anteriores, pero ahora observe las otras estadísticas (como la utilización de CPU y las estadísticas de E/S de bloques). ¿Cómo cambian cuando `mem` se está ejecutando?

**Cómo ejecutarlo**

Repita exactamente las ejecuciones de la pregunta 3, pero ahora observe en `vmstat` las columnas de `cpu` (`us`, `sy`, `id`, `wa`) y de E/S de bloques (`bi`, `bo`).

#### Pregunta 5

Examine ahora el desempeño. Elija una entrada para `mem` que quepa cómodamente en la memoria (por ejemplo, 4000 si la cantidad de memoria del sistema es de 8 GB). ¿Cuánto tarda el loop 0 (y los loops subsiguientes 1, 2, etc.)? Ahora elija un tamaño que esté cómodamente más allá del tamaño de la memoria (por ejemplo, 12000, nuevamente suponiendo 8 GB de memoria). ¿Cuánto tardan los loops en este caso? ¿Cómo se comparan los números de ancho de banda (bandwidth)? ¿Cuán diferente es el desempeño cuando se realiza swapping constantemente frente a cuando todo cabe cómodamente en la memoria? ¿Puede hacer una gráfica, con el tamaño de la memoria usada por `mem` en el eje x y el ancho de banda de acceso a dicha memoria en el eje y? Finalmente, ¿cómo se compara el desempeño del primer loop con el de los loops subsiguientes, tanto para el caso en que todo cabe en la memoria como para el caso en que no?

**Cómo ejecutarlo**

*a) Medición manual.* Elija un tamaño que quepa cómodamente en la memoria y ejecute:

```sh
./mem 4000
```

Lea en la salida el tiempo del loop 0 y de los loops siguientes. Repita con un tamaño claramente mayor que la memoria (por ejemplo, `./mem 12000` con 8 GB).

Si `mem` imprime solo ciertos loops, es normal: el programa imprime una muestra de los loops (a lo sumo una línea cada 0,2 segundos).

*b) Barrido automático para la gráfica.* El siguiente script (`CÓDIGO_NUEVO`) ejecuta `mem` con cada tamaño indicado, lo detiene después de `SEGUNDOS` segundos y registra el ancho de banda del último loop impreso en `bw.csv`. Guárdelo como `bw.sh`:

```sh
#!/bin/bash
# Uso: ./bw.sh tam1 tam2 ...   (tamaños en MB)
# Para cada tamaño, ejecuta mem durante SEGUNDOS y guarda el ancho de banda
# del último loop impreso en bw.csv (tamano_mb,ancho_banda_mb_s)
SEGUNDOS=${SEGUNDOS:-10}
echo "tamano_mb,ancho_banda_mb_s" > bw.csv
for t in "$@"; do
    stdbuf -oL ./mem "$t" > salida_$t.txt 2>&1 &
    pid=$!
    sleep "$SEGUNDOS"
    kill -TERM "$pid" 2>/dev/null
    wait "$pid" 2>/dev/null
    bw=$(grep '^loop' salida_$t.txt | tail -1 | sed 's/.*bandwidth: \([0-9.]*\) MB\/s.*/\1/')
    echo "$t,$bw" >> bw.csv
done
cat bw.csv
```

Ejecútelo así (ajuste los tamaños a la memoria de su sistema y aumente `SEGUNDOS` cuando haya swapping, porque cada loop tarda mucho más):

```sh
chmod +x bw.sh
SEGUNDOS=20 ./bw.sh 1000 2000 4000 6000 8000 10000 12000
```

Grafique `bw.csv` con la herramienta de su preferencia (tamaño en el eje x, ancho de banda en el eje y). Para comparar el primer loop con los siguientes, revise el archivo de salida de cada tamaño:

```sh
grep '^loop' salida_4000.txt
```

> **Importante:** `stdbuf -oL` es necesario. Cuando la salida de `mem` se redirige a un archivo, el programa la acumula en un buffer y, al detenerlo, ese contenido se pierde: el archivo queda vacío.

#### Pregunta 6

El espacio de swap no es infinito. Puede emplear la herramienta `swapon` con la opción `-s` para ver cuánto espacio de swap está disponible. ¿Qué sucede si intenta ejecutar `mem` con valores cada vez más grandes, más allá de lo que parece estar disponible en el espacio de swap? ¿En qué punto falla la asignación de memoria?

**Cómo ejecutarlo**

Consulte el espacio de swap disponible y la memoria libre:

```sh
swapon -s
free -m
```

Ejecute `mem` con valores cada vez mayores, uno a la vez:

```sh
./mem 8000
./mem 10000
./mem 12000
```

Cuando la asignación falla, `mem` imprime `memory allocation failed` y termina. Para ver su código de salida inmediatamente después:

```sh
echo $?
```

Registre el tamaño a partir del cual ocurre la falla y compárelo con la memoria total más el espacio de swap.

#### Pregunta 7

Finalmente, si usted tiene un nivel avanzado, puede configurar su sistema para usar diferentes dispositivos de swap mediante `swapon` y `swapoff`. Lea las man pages para conocer los detalles. Si tiene acceso a hardware diferente, observe cómo cambia el desempeño del swapping al realizarlo hacia un disco duro clásico, un SSD basado en flash e incluso un arreglo RAID. ¿Cuánto puede mejorarse el desempeño del swapping mediante dispositivos más nuevos? ¿Qué tan cerca puede llegar del desempeño en memoria?

**Cómo ejecutarlo**

Consulte los dispositivos de swap activos y las man pages:

```sh
swapon --show
man swapon
man swapoff
```

Requiere privilegios de administrador. Para desactivar un dispositivo y activar otro:

```sh
sudo swapoff /dev/NOMBRE_DEL_DISPOSITIVO_ACTUAL
sudo swapon /dev/NOMBRE_DEL_DISPOSITIVO_NUEVO
```

> **Cuidado:** el dispositivo nuevo debe estar preparado como área de swap (`mkswap`), operación que **destruye** los datos de ese dispositivo. Verifique el nombre del dispositivo antes de ejecutarla.

Para cada dispositivo (disco duro clásico, SSD, arreglo RAID), repita la medición de la pregunta 5 con el barrido `bw.sh`.

### Referencias

- Arpaci-Dusseau, R. H. y Arpaci-Dusseau, A. C. *Operating Systems: Three Easy Pieces*. Capítulo 21, "Beyond Physical Memory: Mechanisms": <https://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys.pdf>
- Tarea original (Measurement) y código fuente de `mem.c`: <https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/vm-beyondphys>