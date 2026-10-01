## Guía de compilación y ejecución

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

> **Advertencia:** ejecutar `mem` con tamaños cercanos o superiores a la memoria del sistema puede volverlo muy lento o provocar que el sistema operativo finalice procesos. Se recomienda hacerlo en una máquina virtual o en un equipo donde no haya trabajo sin guardar.

### Pregunta 1: uso de CPU con poca memoria

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

### Pregunta 2: columnas `swpd` y `free`

Terminal A: `vmstat 1` (o `vmstat -S M 1` para mostrar las columnas de memoria en MB).

Terminal B:

```sh
./mem 1024
```

Observe `swpd` y `free` mientras el programa se ejecuta. Luego detenga el programa con control-c y siga observando las mismas columnas.

### Pregunta 3: columnas `si` y `so`

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

Continúe con tamaños mayores. Observe las columnas `si` y `so` de `vmstat`, y compare lo que sucede durante el primer ciclo (`loop 0`) con los ciclos siguientes en la salida de `mem`.

### Pregunta 4: otras estadísticas

Repita exactamente las ejecuciones de la pregunta 3, pero ahora observe en `vmstat` las columnas de `cpu` (`us`, `sy`, `id`, `wa`) y de E/S de bloques (`bi`, `bo`).

### Pregunta 5: desempeño y gráfica de ancho de banda

**a) Medición manual.** Elija un tamaño que quepa cómodamente en la memoria y ejecute:

```sh
./mem 4000
```

Lea en la salida el tiempo del ciclo 0 y de los ciclos siguientes. Repita con un tamaño claramente mayor que la memoria (por ejemplo, `./mem 12000` con 8 GB).

Si `mem` imprime solo ciertos ciclos, es normal: el programa imprime una muestra de los ciclos (a lo sumo una línea cada 0,2 segundos).

**b) Barrido automático para la gráfica.** El siguiente script (`CÓDIGO_NUEVO`) ejecuta `mem` con cada tamaño indicado, lo detiene después de `SEGUNDOS` segundos y registra el ancho de banda del último ciclo impreso en `bw.csv`. Guárdelo como `bw.sh`:

```sh
#!/bin/bash
# Uso: ./bw.sh tam1 tam2 ...   (tamaños en MB)
# Para cada tamaño, ejecuta mem durante SEGUNDOS y guarda el ancho de banda
# del último ciclo impreso en bw.csv (tamano_mb,ancho_banda_mb_s)
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

Ejecútelo así (ajuste los tamaños a la memoria de su sistema y aumente `SEGUNDOS` cuando haya swapping, porque cada ciclo tarda mucho más):

```sh
chmod +x bw.sh
SEGUNDOS=20 ./bw.sh 1000 2000 4000 6000 8000 10000 12000
```

Grafique `bw.csv` con la herramienta de su preferencia (tamaño en el eje x, ancho de banda en el eje y). Para comparar el primer ciclo con los siguientes, revise el archivo de salida de cada tamaño:

```sh
grep '^loop' salida_4000.txt
```

> **Importante:** `stdbuf -oL` es necesario. Cuando la salida de `mem` se redirige a un archivo, el programa la acumula en un buffer y, al detenerlo, ese contenido se pierde: el archivo queda vacío.

### Pregunta 6: límite del espacio de swap

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

### Pregunta 7: dispositivos de swap diferentes

Consulte los dispositivos de swap activos y las páginas del manual:

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