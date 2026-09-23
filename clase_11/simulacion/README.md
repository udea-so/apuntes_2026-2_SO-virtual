![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Práctica guiada: Medición empírica de la TLB

**Curso:** Sistemas Operativos — UdeA
**Tema:** TLB (Translation Lookaside Buffer)

## Objetivo

Medir de forma empírica, mediante un experimento controlado de acceso a
memoria, el tamaño de la TLB (o de sus distintos niveles) de su procesador y
la penalización aproximada de un TLB miss en cada nivel — sin usar
contadores de hardware ni documentación del fabricante, solo cronometrando
accesos a memoria con precisión.

Al finalizar esta práctica, usted debe ser capaz de:

- Explicar por qué recorrer un arreglo tocando una sola dirección por
  página revela el comportamiento de la TLB.
- Identificar, en una gráfica generada con sus propios datos, los límites
  de cada nivel de TLB de su computador.
- Reconocer y controlar las fuentes de error que pueden arruinar una
  medición de este tipo: precisión del temporizador, migración del proceso
  entre CPUs, costos de inicialización de memoria y optimización del
  compilador.

## Conceptos clave antes de empezar

Antes de ejecutar cualquier instrucción, asegúrese de comprender estos
términos. Las preguntas de la práctica los dan por conocidos.

- **TLB (Translation Lookaside Buffer):** una caché pequeña y rápida,
  dentro del CPU, que guarda las traducciones de dirección virtual a física
  usadas más recientemente. Evita tener que recorrer la tabla de páginas en
  memoria en cada acceso.
- **TLB hit / TLB miss:** un *hit* ocurre cuando la traducción necesaria ya
  está en la TLB (rápido); un *miss* ocurre cuando no está y hay que
  buscarla en la tabla de páginas en memoria (mucho más lento).
- **VPN / PFN / página:** una dirección virtual se divide en un número de
  página virtual (VPN) y un desplazamiento (*offset*); la traducción
  produce un número de marco físico (PFN). El tamaño de página determina
  cuántas direcciones caen en la misma entrada de TLB.
- **Jerarquía de TLB:** al igual que la caché de datos, la TLB puede tener
  varios niveles (L1-TLB, L2-TLB), cada uno más grande pero más lento que
  el anterior.
- **Ruido de medición y precisión de un temporizador de software:** un
  TLB hit o miss dura pocos nanosegundos; la propia llamada al temporizador
  del sistema operativo tiene un costo que puede ser del mismo orden de
  magnitud. Medir bien implica diseñar el experimento para que ese costo no
  contamine el resultado.
- **Afinidad de CPU (CPU pinning):** cada núcleo de un procesador
  multi-core tiene su propia TLB física. Si el planificador del sistema
  operativo mueve un proceso de un núcleo a otro durante la medición, el
  proceso puede "heredar" una TLB con contenido distinto, distorsionando
  los resultados.
- **Costo de inicialización (demand zeroing):** la primera vez que se
  accede a una página recién reservada, el sistema operativo debe
  inicializarla (llenarla de ceros) antes de entregarla, lo cual es mucho
  más costoso que un acceso posterior a la misma página.
- **Optimización del compilador:** un compilador puede eliminar o
  reordenar código cuyo resultado nunca se usa. Un experimento de
  temporización mal escrito puede terminar midiendo un bucle vacío sin que
  el programador lo note.

## Requisitos

- `gcc` (o cualquier compilador C99/C11 compatible con POSIX/GNU:
  `clock_gettime`, `posix_memalign`, `sysconf`, `sched_setaffinity`).
- `python3` con `matplotlib` instalado (`pip install matplotlib` si hace
  falta).
- `sched_setaffinity` es específico de Linux (glibc). En otros sistemas
  operativos la fijación de CPU puede fallar; el programa lo advierte por
  pantalla y continúa de todas formas.

## Archivos

| Archivo | Contenido |
|---|---|
| `tlb_bench.c` | El programa de medición. Sus comentarios explican cada decisión de diseño. |
| `Makefile` | Automatiza la compilación, ejecución y graficación. |
| `plot_tlb.py` | Genera la gráfica (PNG) a partir del CSV de resultados. |
| `README.md` | Esta guía. |

## Instrucciones

Siga estos pasos en orden. No necesita modificar el código para
completarlos — solo para las preguntas que se lo pidan explícitamente.

1. Verifique que tiene instalados `gcc` y `python3` con `matplotlib`.
2. Ubique `tlb_bench.c`, `Makefile` y `plot_tlb.py` en un mismo directorio.
3. Compile el programa:
   ```bash
   make
   ```
4. Ejecute el barrido de medición y genere la gráfica en un solo paso:
   ```bash
   make plot
   ```
   Esto produce `resultados.csv` (los datos crudos) y `resultados.png` (la
   gráfica) en el mismo directorio.
5. Abra `resultados.png` y revise la forma general de la curva antes de
   continuar con las preguntas.

Para ejecuciones adicionales con parámetros distintos (necesarias para
varias de las preguntas), use el programa directamente:

```bash
./tlb_bench [-m paginas_min] [-M paginas_max] [-r repeticiones] \
            [-c cpu_id] [-a accesos_objetivo] [-o salida.csv]
python3 plot_tlb.py salida.csv salida.png
```

| Parámetro | Significado | Valor por defecto |
|---|---|---|
| `-m` | Número mínimo de páginas del barrido (potencia de 2) | 1 |
| `-M` | Número máximo de páginas del barrido (potencia de 2) | 65536 (256MB con páginas de 4KB) |
| `-r` | Repeticiones cronometradas por punto (se reporta el mínimo) | 7 |
| `-c` | CPU al que se fija el proceso; `-1` desactiva la fijación | 0 |
| `-a` | Accesos mínimos a acumular por medición | 200000 |
| `-o` | Archivo CSV de salida | stdout |

Ejecute `./tlb_bench -h` en cualquier momento para ver esta misma tabla en
la terminal. Los mismos parámetros están disponibles como variables del
`Makefile` (por ejemplo, `make plot CPU=-1`).

Si su equipo tiene poca memoria disponible, reduzca `-M` (por ejemplo,
`-M 16384` para limitar el barrido a 64MB).

> **Nota de diseño:** el barrido de páginas está integrado directamente
> dentro de `tlb_bench.c` (no requiere un script externo que invoque el
> programa muchas veces), para evitar el costo de lanzar un proceso nuevo
> en cada punto de medición.

## Cómo interpretar la gráfica

El eje X es el número de páginas tocadas (escala logarítmica en base 2); el
eje Y es el tiempo promedio por acceso, en nanosegundos. La curva esperada
es escalonada:

- Un primer tramo bajo y plano, mientras todas las traducciones caben en la
  TLB de primer nivel: su extensión indica el tamaño de esa TLB.
- Un salto hacia un segundo tramo más alto: se agotó la TLB de primer nivel,
  pero las traducciones aún caben en una TLB de segundo nivel (si el
  procesador tiene una).
- Un segundo salto, considerablemente más alto: se agotó también la TLB de
  segundo nivel, y cada acceso exige recorrer la tabla de páginas en
  memoria.

`plot_tlb.py` marca automáticamente, sobre la propia gráfica, los saltos
que detecta (una razón entre accesos consecutivos superior a 1.4x).

## Preguntas

Responda las siguientes preguntas con base en las ejecuciones que realice.
Documente los comandos usados y los datos obtenidos para justificar cada
respuesta — no basta con una conclusión sin evidencia.

### Sobre el diseño de la medición

1. Ejecute el programa con `-a 1` (sin acumulación interna de accesos) y
   luego con el valor por defecto (`-a 200000`), ambos con `-M 64`.
   Compare el tiempo reportado para `numpaginas=1` en los dos casos. ¿Qué
   le indica esa diferencia sobre el costo de una sola llamada al
   temporizador del sistema, frente al costo real de un acceso a memoria?
2. Observe los parámetros de entrada del programa (`-m`, `-M`, `-r`). ¿Por
   qué considera que el barrido se expresa en número de páginas, y no en
   número de bytes o en duración total del experimento?
3. Ejecute el mismo barrido con `-r 1`, `-r 7` y `-r 30` (puede usar
   `-M 4096` para reducir el tiempo total). ¿A partir de qué número de
   repeticiones los resultados dejan de cambiar de forma apreciable en su
   equipo? Justifique con los datos obtenidos, no a partir de una
   impresión visual.
4. Compare el archivo `resultados.csv` con `resultados.png`. ¿En qué
   momento logra identificar el tamaño de la TLB en cada formato? ¿Por qué
   la escala logarítmica en base 2 del eje X es apropiada para este
   experimento en particular?
5. Genere una copia de `tlb_bench.c` sin la palabra clave `volatile` en el
   puntero del arreglo (o reemplazando `arreglo[i] += 1` por una lectura
   simple cuyo resultado no se usa). Compile ambas versiones y compare el
   ensamblador generado con `objdump -d`. ¿Se conserva el bucle principal
   en ambos casos? ¿Cambia el tiempo medido? Explique qué hizo el
   compilador y por qué.
6. Ejecute el barrido fijando el proceso al CPU 0 (`-c 0`, valor por
   defecto) y luego sin fijarlo (`-c -1`), en un equipo con varios núcleos
   y alguna carga adicional en segundo plano. Compare la estabilidad de
   los resultados entre ambas ejecuciones. ¿Es consistente con lo que
   esperaría si cada núcleo tuviera su propia TLB?
7. Modifique temporalmente `tlb_bench.c` comentando el bloque que
   inicializa todas las páginas del arreglo antes del barrido. Recompile y
   ejecute. ¿Qué le ocurre a la medición del punto con menos páginas?
   Relacione lo observado con el costo de inicializar una página por
   primera vez.

### Sobre los resultados obtenidos

8. ¿Cuántos niveles de TLB identifica en su computador, a partir de los
   saltos observados en la gráfica? ¿En qué número de páginas ocurre cada
   uno?
9. A partir de esos saltos, ¿cuántas entradas tiene aproximadamente la TLB
   de primer nivel de su CPU? Consulte la especificación real de su
   procesador (`lscpu`, la ficha técnica del fabricante, o una herramienta
   como `cpuid`) y compare ambos valores.
10. ¿Cuál es la penalización aproximada, en nanosegundos y como factor
    multiplicativo, de un TLB miss en cada nivel identificado?
11. Repita el experimento aumentando `-M` lo suficiente para que el
    barrido supere el tamaño de la caché L2/L3 de su CPU. ¿Observa un
    salto adicional? ¿Cómo distinguiría si ese salto corresponde a la TLB
    o a la caché de datos?
12. (Opcional, en Linux) Desactive temporalmente *Transparent Huge Pages* y
    repita la medición:
    ```bash
    cat /sys/kernel/mm/transparent_hugepage/enabled
    echo madvise | sudo tee /sys/kernel/mm/transparent_hugepage/enabled
    ```
    ¿Cambian los saltos observados respecto a su medición original?
    Explique por qué.

## Limitaciones conocidas

Documente en su informe cualquiera de estas limitaciones que haya afectado
sus resultados; no las omita.

- **Ruido de medición:** el sistema operativo, otros procesos y el ajuste
  dinámico de frecuencia del CPU introducen variabilidad. El programa la
  mitiga tomando el mínimo de varias repeticiones y acumulando suficientes
  accesos por medición, pero repetir el experimento en un equipo lo más
  "quieto" posible da resultados más confiables.
- **Migración entre CPUs:** sin fijación de CPU, el planificador puede
  mover el proceso entre núcleos, mezclando TLBs distintas en la misma
  medición. La fijación está activada por defecto, pero puede fallar
  silenciosamente en algunos entornos (el programa lo advierte por
  pantalla si ocurre).
- **Transparent Huge Pages:** si el sistema operativo usa páginas de 2MB
  para el arreglo, el tamaño de página asumido (4KB) no coincide con el
  real, y los saltos no aparecen donde se esperaría.
- **Confusión con la caché de datos:** a partir de cierto tamaño de
  arreglo, además de TLB misses pueden empezar a aparecer misses de caché
  de datos (L2/L3), mezclados en la misma medición.
- **Máquinas virtuales o contenedores:** en entornos virtualizados la
  traducción de direcciones pasa por una capa adicional, y los resultados
  pueden ser más ruidosos o menos definidos que en una máquina física.

## Referencias

- Saavedra-Barrera, R. H. (1992). *CPU Performance Evaluation and Execution
  Time Prediction Using Narrow Spectrum Benchmarking.* University of
  California, Berkeley, Technical Report UCB/CSD-92-684.
- Arpaci-Dusseau, R. H., & Arpaci-Dusseau, A. C. *Operating Systems: Three
  Easy Pieces*, Capítulo 19, "Paging: Faster Translations (TLBs)".
  <https://pages.cs.wisc.edu/~remzi/OSTEP/vm-tlbs.pdf>
