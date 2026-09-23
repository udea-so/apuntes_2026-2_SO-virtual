/*
 * tlb_bench.c
 * ---------------------------------------------------------------------------
 * Medición empírica de la TLB (Translation Lookaside Buffer) siguiendo el
 * método de Saavedra-Barrera:
 *
 *   Saavedra-Barrera, R. H. "CPU Performance Evaluation and Execution Time
 *   Prediction Using Narrow Spectrum Benchmarking." UC Berkeley, Technical
 *   Report UCB/CSD-92-684, febrero de 1992.
 *
 * Este archivo implementa el "Homework" tal como lo plantea OSTEP,
 * Capítulo 19 "Paging: Faster Translations (TLBs)"
 * (https://pages.cs.wisc.edu/~remzi/OSTEP/vm-tlbs.pdf). El homework real de
 * Remzi tiene 7 preguntas, y cada una corresponde a una decisión de diseño
 * concreta de este código. Se listan aquí para que quede explícito dónde se
 * resuelve cada una (los números [Rn] se repiten como comentarios en el
 * código más abajo):
 *
 *   [R1] Precisión del timer: ¿qué tan preciso es? ¿cuántas repeticiones
 *        hacen falta para medir con precisión?
 *        -> Resuelto con repeticiones INTERNAS por pasada, ver
 *           'accesos_objetivo' / una_pasada_ns() más abajo.
 *
 *   [R2] Escribir el programa (aquí: tlb_bench.c) con inputs: número de
 *        páginas a tocar y número de trials (aquí: -r).
 *        -> Este archivo + los parámetros -m/-M/-r de main().
 *
 *   [R3] Escribir un script que varíe el número de páginas de 1 a unos
 *        miles (factor de 2), correrlo en varias máquinas, y determinar
 *        cuántos trials hacen falta para mediciones confiables.
 *        -> Aquí el barrido NO es un script externo, está integrado
 *           directamente en el bucle principal de main() (ver el 'for' que
 *           recorre numpaginas *= 2). Es una decisión de diseño distinta a
 *           la literal de Remzi (evita el overhead de lanzar un proceso
 *           nuevo por cada punto), pero cumple el mismo propósito. El
 *           Makefile expone -r para experimentar con el número de trials.
 *
 *   [R4] Graficar los resultados; reflexionar por qué la visualización
 *        ayuda a digerir los datos.
 *        -> plot_tlb.py (ver README.md).
 *
 *   [R5] Cuidado con la optimización del compilador: puede eliminar bucles
 *        que incrementan valores que nadie más usa.
 *        -> 'arreglo[i] += 1' (no solo lectura) más el calificador
 *           'volatile' en el puntero. Ver advertencia #1 más abajo.
 *
 *   [R6] Cuidado con sistemas multi-CPU: cada CPU tiene su propia jerarquía
 *        de TLB, así que si el scheduler mueve el proceso de un CPU a otro
 *        durante la medición, los resultados se contaminan. Hay que fijar
 *        el proceso a un solo CPU ("pinning").
 *        -> función fijar_cpu(), usa sched_setaffinity(). Parámetro -c.
 *
 *   [R7] Cuidado con el costo de inicialización (demand zeroing): si no se
 *        inicializa el arreglo antes de medir, el primer acceso a cada
 *        página es artificialmente caro.
 *        -> prefault de TODO el bloque en main() antes de cualquier
 *           medición, más el warm-up individual en medir_ns_por_acceso().
 *
 * IDEA CENTRAL DEL EXPERIMENTO
 * ---------------------------------------------------------------------------
 * Se recorre un arreglo tocando exactamente UN entero por página:
 *
 *      salto = TAMANIO_PAGINA / sizeof(int);
 *      for (i = 0; i < NUMPAGES * salto; i += salto)
 *          arreglo[i] += 1;
 *
 * Cada iteración cae en una página distinta, así que cada acceso exige una
 * traducción de dirección (VPN -> PFN) nueva. Si el número de páginas
 * tocadas (NUMPAGES) es menor que la capacidad de la TLB, todas las
 * traducciones son TLB hits. En cuanto NUMPAGES supera esa capacidad,
 * empiezan a aparecer TLB misses y el tiempo promedio por acceso sube.
 *
 * Al medir el tiempo por acceso para NUMPAGES = 1, 2, 4, 8, ... (potencias
 * de 2) y graficarlo, aparecen "escalones": cada escalón marca el límite de
 * un nivel de la TLB (L1-TLB, L2-TLB, etc.), y la altura del escalón es la
 * penalización aproximada de un miss en ese nivel.
 *
 * ADVERTENCIAS / LIMITACIONES A TENER EN CUENTA (ver también README.md)
 * ---------------------------------------------------------------------------
 *  1. [R5] Optimización del compilador: usamos "arreglo[i] += 1" (no solo
 *     lectura) porque el efecto secundario de escritura evita que el
 *     compilador elimine el bucle incluso con -O2. Además el puntero se
 *     declara 'volatile' para impedir que el compilador vectorice o
 *     reordene los accesos de forma que el experimento deje de medir "un
 *     acceso a la vez". (Pregunta guía: verifiquen esto ustedes mismos
 *     quitando 'volatile' y mirando el ensamblador generado con
 *     `objdump -d tlb_bench` — ver README.md.)
 *  2. [R6] Multi-CPU / TLB por núcleo: por defecto el proceso se fija al
 *     CPU 0 con sched_setaffinity() (ver fijar_cpu()). Usar -c -1 para
 *     desactivar el pinning y comparar qué tan ruidosos quedan los
 *     resultados sin él.
 *  3. [R7] Costo de inicialización: se prefaultan TODAS las páginas del
 *     bloque una sola vez al inicio (demand zeroing) para que ese costo no
 *     se mezcle con ninguna medición.
 *  4. [R1] Precisión del timer: clock_gettime() tiene su propio costo fijo,
 *     que puede dominar el tiempo medido si NUMPAGES es chico. Por eso cada
 *     pasada cronometrada repite el recorrido internamente hasta acumular
 *     al menos 'accesos_objetivo' accesos (ver medir_ns_por_acceso()).
 *  5. Transparent Huge Pages (THP) en Linux: si el kernel decide usar
 *     páginas grandes (2MB) para este arreglo, el tamaño de página real
 *     usado en la traducción no coincide con sysconf(_SC_PAGESIZE) (4KB) y
 *     los escalones observados no corresponden a lo esperado. Ver el README
 *     para instrucciones sobre cómo desactivar THP para este experimento.
 *  6. Confusión con la caché de datos: este mismo experimento (variando
 *     también el "stride") es el que se usa para medir el tamaño de la
 *     caché. Aquí el stride está fijo en una página, así que a partir de
 *     cierto NUMPAGES es posible que también empecemos a ver misses de
 *     caché de datos (L2/L3) mezclados con los TLB misses.
 *
 * COMPILACIÓN
 * ---------------------------------------------------------------------------
 *   gcc -O2 -Wall -Wextra -std=c11 tlb_bench.c -o tlb_bench
 *
 * USO
 * ---------------------------------------------------------------------------
 *   ./tlb_bench [-m paginas_min] [-M paginas_max] [-r repeticiones]
 *               [-c cpu_id] [-a accesos_objetivo] [-o archivo_salida.csv]
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

/* ---------------------------------------------------------------------- */
/* [R6] Fijar el proceso a un solo CPU ("pinning")                         */
/* ---------------------------------------------------------------------- */

/*
 * Cada CPU tiene su propia jerarquía de TLB. Si el planificador del SO
 * mueve este proceso de un CPU a otro durante la medición, el proceso
 * "hereda" una TLB fría en el nuevo CPU (o una con contenido de otro
 * proceso), lo cual contamina la medición con misses que no tienen que ver
 * con el tamaño real de NUMPAGES.
 *
 * cpu_id < 0 desactiva el pinning (para comparar con/sin, ver pregunta
 * guía en README.md). Si sched_setaffinity() falla (p. ej. por falta de
 * permisos en un contenedor), se imprime un aviso pero el programa
 * continua: la medición sigue siendo valida, solo puede ser mas ruidosa.
 */
static void fijar_cpu(int cpu_id) {
    if (cpu_id < 0) {
        fprintf(stderr, "# Pinning de CPU desactivado (-c -1)\n");
        return;
    }

    cpu_set_t conjunto;
    CPU_ZERO(&conjunto);
    CPU_SET((unsigned)cpu_id, &conjunto);

    if (sched_setaffinity(0, sizeof(conjunto), &conjunto) != 0) {
        fprintf(stderr,
            "# Aviso: no se pudo fijar el proceso al CPU %d (%s).\n"
            "#        Los resultados pueden ser mas ruidosos si el\n"
            "#        planificador mueve el proceso entre CPUs (ver\n"
            "#        pregunta [R6] del homework en README.md).\n",
            cpu_id, strerror(errno));
    } else {
        fprintf(stderr, "# Proceso fijado al CPU %d\n", cpu_id);
    }
}

/* ---------------------------------------------------------------------- */
/* Utilidades de tiempo                                                    */
/* ---------------------------------------------------------------------- */

/* Devuelve el reloj monótono actual en nanosegundos. CLOCK_MONOTONIC no se
 * ve afectado por ajustes del reloj del sistema (NTP, cambios de hora), lo
 * cual es justo lo que queremos para medir duraciones. */
static double ahora_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

/* ---------------------------------------------------------------------- */
/* Núcleo del benchmark                                                    */
/* ---------------------------------------------------------------------- */

/*
 * Recorre 'numpaginas' páginas del arreglo, tocando un entero por página,
 * y repite ese recorrido 'iteraciones_internas' veces seguidas DENTRO de
 * la misma ventana cronometrada. Devuelve el tiempo total (en ns) de todas
 * las repeticiones internas juntas.
 *
 * 'arreglo' se declara volatile para impedir que el compilador reordene,
 * agrupe o elimine los accesos individuales [R5].
 */
static double una_pasada_ns(volatile int *arreglo, size_t numpaginas, size_t salto,
                             size_t iteraciones_internas) {
    double t0 = ahora_ns();
    for (size_t it = 0; it < iteraciones_internas; it++) {
        for (size_t i = 0; i < numpaginas * salto; i += salto) {
            arreglo[i] += 1;
        }
    }
    double t1 = ahora_ns();
    return t1 - t0;
}

/*
 * Mide el tiempo promedio por acceso (en ns) para un NUMPAGES dado.
 *
 * Estrategia:
 *   - [R1] 'iteraciones_internas' se calcula para que cada pasada
 *     cronometrada acumule al menos 'accesos_objetivo' accesos en total.
 *     clock_gettime() tiene un costo fijo (una llamada al sistema, o con
 *     vDSO una lectura de memoria mapeada) que, para NUMPAGES chico, puede
 *     ser comparable o mayor al costo real de los accesos que queremos
 *     medir. Repetir el recorrido completo varias veces DENTRO de la misma
 *     ventana cronometrada diluye ese costo fijo entre muchos más accesos,
 *     en vez de pagarlo una vez por cada NUMPAGES chico. Este es
 *     exactamente el problema que plantea la pregunta [R1] del homework:
 *     "¿qué tan preciso es el timer? ¿cuántas repeticiones hacen falta?".
 *   - [R7] Se hace una pasada de "calentamiento" (warm-up) que NO se
 *     cronometra, para que los page faults iniciales (primer toque de cada
 *     página) no contaminen la medición: nos interesa el costo de la
 *     traducción de direcciones en estado estable, no el costo de poblar
 *     las tablas de página por primera vez.
 *   - Se hacen 'repeticiones' pasadas cronometradas y se toma el MÍNIMO:
 *     el ruido del sistema (interrupciones, otros procesos, frequency
 *     scaling) solo puede AUMENTAR el tiempo medido por encima del costo
 *     real del hardware, nunca disminuirlo por debajo. El mínimo es, por
 *     lo tanto, la mejor estimación del costo real.
 */
static double medir_ns_por_acceso(volatile int *arreglo, size_t numpaginas,
                                   size_t salto, int repeticiones,
                                   size_t accesos_objetivo) {
    size_t iteraciones_internas = accesos_objetivo / numpaginas;
    if (iteraciones_internas < 1) {
        iteraciones_internas = 1;
    }

    /* [R7] Calentamiento: fuerza los page faults fuera de la medición. */
    una_pasada_ns(arreglo, numpaginas, salto, 1);

    double mejor_ns = -1.0;
    for (int r = 0; r < repeticiones; r++) {
        double t = una_pasada_ns(arreglo, numpaginas, salto, iteraciones_internas);
        if (mejor_ns < 0.0 || t < mejor_ns) {
            mejor_ns = t;
        }
    }
    return mejor_ns / (double)(numpaginas * iteraciones_internas);
}

/* ---------------------------------------------------------------------- */
/* Línea de comandos                                                       */
/* ---------------------------------------------------------------------- */

static void imprimir_ayuda(const char *prog) {
    fprintf(stderr,
        "Uso: %s [-m paginas_min] [-M paginas_max] [-r repeticiones]\n"
        "          [-c cpu_id] [-a accesos_objetivo] [-o salida.csv]\n"
        "\n"
        "  -m  Numero minimo de paginas a recorrer (potencia de 2). Default: 1\n"
        "  -M  Numero maximo de paginas a recorrer (potencia de 2). Default: 65536\n"
        "  -r  Repeticiones cronometradas por punto (se toma el minimo). Default: 7\n"
        "  -c  CPU al que se fija el proceso (sched_setaffinity). -1 desactiva\n"
        "      el pinning. Default: 0  [pregunta R6 del homework]\n"
        "  -a  Accesos minimos a acumular por pasada cronometrada, para diluir\n"
        "      el costo fijo del timer. Default: 200000  [pregunta R1 del homework]\n"
        "  -o  Archivo CSV de salida. Default: stdout\n"
        "  -h  Muestra esta ayuda\n"
        "\n"
        "Metodo de Saavedra-Barrera para medir la TLB (homework de OSTEP\n"
        "capitulo 19). Ver los comentarios al inicio de tlb_bench.c y\n"
        "README.md para el mapeo completo con las 7 preguntas del homework.\n",
        prog);
}

int main(int argc, char **argv) {
    size_t paginas_min = 1;
    size_t paginas_max = 65536; /* 64K paginas * 4KB = 256MB por defecto */
    int repeticiones = 7;
    int cpu_id = 0;
    size_t accesos_objetivo = 200000;
    const char *ruta_salida = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "m:M:r:c:a:o:h")) != -1) {
        switch (opt) {
            case 'm': paginas_min = (size_t)strtoull(optarg, NULL, 10); break;
            case 'M': paginas_max = (size_t)strtoull(optarg, NULL, 10); break;
            case 'r': repeticiones = atoi(optarg); break;
            case 'c': cpu_id = atoi(optarg); break;
            case 'a': accesos_objetivo = (size_t)strtoull(optarg, NULL, 10); break;
            case 'o': ruta_salida = optarg; break;
            case 'h':
            default:
                imprimir_ayuda(argv[0]);
                return (opt == 'h') ? 0 : 1;
        }
    }

    if (paginas_min == 0 || paginas_max < paginas_min || repeticiones <= 0 ||
        accesos_objetivo == 0) {
        fprintf(stderr, "Error: parametros invalidos.\n");
        imprimir_ayuda(argv[0]);
        return 1;
    }

    /* [R6] Fijar CPU antes de reservar memoria y medir. */
    fijar_cpu(cpu_id);

    long tam_pagina_l = sysconf(_SC_PAGESIZE);
    if (tam_pagina_l <= 0) {
        fprintf(stderr, "Error: no se pudo obtener el tamano de pagina (sysconf).\n");
        return 1;
    }
    size_t tam_pagina = (size_t)tam_pagina_l;
    size_t salto = tam_pagina / sizeof(int);

    /* Reservamos de una sola vez el arreglo mas grande que vamos a necesitar,
     * alineado exactamente a un limite de pagina, y lo reutilizamos para
     * todos los tamanios NUMPAGES del barrido (asi el arreglo "vive" en las
     * mismas direcciones fisicas/virtuales durante todo el experimento). */
    size_t bytes_totales = paginas_max * tam_pagina;
    void *bloque = NULL;
    int rc = posix_memalign(&bloque, tam_pagina, bytes_totales);
    if (rc != 0 || bloque == NULL) {
        fprintf(stderr, "Error: posix_memalign fallo (%s) reservando %zu bytes.\n",
                strerror(rc), bytes_totales);
        return 1;
    }
    volatile int *arreglo = (volatile int *)bloque;

    /* [R7] Prefaultamos TODO el bloque una sola vez al inicio, para que los
     * page faults de primer toque (demand zeroing) no se mezclen con la
     * medicion de ningun punto del barrido. */
    for (size_t i = 0; i < paginas_max * salto; i += salto) {
        arreglo[i] = 0;
    }

    FILE *salida = stdout;
    if (ruta_salida != NULL) {
        salida = fopen(ruta_salida, "w");
        if (salida == NULL) {
            fprintf(stderr, "Error: no se pudo abrir '%s' para escritura.\n", ruta_salida);
            free(bloque);
            return 1;
        }
    }

    fprintf(stderr,
        "# tlb_bench: tamano de pagina = %zu bytes, paginas = [%zu, %zu], "
        "repeticiones = %d, accesos_objetivo = %zu\n",
        tam_pagina, paginas_min, paginas_max, repeticiones, accesos_objetivo);

    fprintf(salida, "numpaginas,bytes,ns_por_acceso\n");

    for (size_t numpaginas = paginas_min; numpaginas <= paginas_max; numpaginas *= 2) {
        double ns = medir_ns_por_acceso(arreglo, numpaginas, salto, repeticiones,
                                         accesos_objetivo);
        size_t bytes = numpaginas * tam_pagina;
        fprintf(salida, "%zu,%zu,%.4f\n", numpaginas, bytes, ns);
        fflush(salida);
        fprintf(stderr, "  %8zu paginas (%10zu bytes): %8.3f ns/acceso\n",
                numpaginas, bytes, ns);

        if (numpaginas > paginas_max / 2) {
            /* evita que numpaginas*2 se desborde o se pase del limite */
            break;
        }
    }

    if (salida != stdout) {
        fclose(salida);
    }
    free(bloque);
    return 0;
}
