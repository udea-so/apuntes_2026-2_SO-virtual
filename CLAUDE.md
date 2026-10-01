# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Qué es este repositorio

Apuntes de clase para el curso de **Sistemas Operativos (Ude@)**, semestre 2026-2. No es un proyecto de software: es una colección de notas de clase, diapositivas y pequeños programas de ejemplo/laboratorio en C y Python usados para ilustrar conceptos del curso (basados en el libro *Operating Systems: Three Easy Pieces* / OSTEP). El `README.md` de la raíz es la tabla de contenidos del curso: enlaza cada clase con su PDF anotado, su carpeta de `apuntes` y, cuando existe, su `apuntes_zoom` y su `simulador/`/`simulacion/`. Está dividido por módulos (Módulo 1 – Virtualización de CPU, Módulo 2 – Virtualización de memoria), con una fila por **sesión** (horario martes y jueves): la columna "Clase" es el número de sesión, no el de la carpeta, y una carpeta `clase_NN/` puede ocupar varias filas ("Parte 1", "Parte 2"). En el Módulo 2 el "Tema" usa el número de carpeta (`Clase 9 - Segmentación` → `clase_09/`); en el Módulo 1 hay un desfase histórico que no se corrige. Lo importante es que cada enlace apunte a una ruta existente y al contenido que nombra la fila.

## Estructura

Cada `clase_NN/` corresponde a una sesión de clase y sigue (aproximadamente) este patrón:

- `SO_apuntes_claseNN.pptx` / `.pdf` / `.xopp` / `_annotated.pdf`: diapositivas y anotaciones manuscritas (Xournal++) de la clase. Archivos binarios, no se editan con Claude.
- `apuntes/`: notas de clase en Markdown, con sus imágenes/GIFs de apoyo y, cuando aplica, el código o simulador asociado a esa clase. Es material teórico curado manualmente por el profesor.
- `apuntes_zoom/`: carpeta hermana de `apuntes/`, dedicada exclusivamente al resumen de la sesión generado combinando el manuscrito anotado con el/los resumen(es) de Zoom de la clase. Se genera siguiendo `prompt_maestro_apuntes_clase_so_v1.2.md` (raíz del repo) — ver esa guía para la plantilla, reglas de fidelidad a la fuente y convención de usar diagramas Mermaid donde ayuden a la pedagogía. No duplica código ni simuladores: solo referencia los ya existentes en `apuntes/` o en la carpeta hermana correspondiente. Existe (un único `README.md` por carpeta) para `clase_02` a `clase_11`; algunas clases se dictaron en más de un día calendario y el propio `apuntes_zoom/README.md` señala con una nota el corte de sesión (p. ej. `clase_04` combina las sesiones del 13/08 y 18/08; `clase_09`, las del 10/09 y 15/09; `clase_10`, las del 15/09, 17/09, 22/09 y 24/09; `clase_11`, las del 24/09 y 29/09). Un `apuntes_zoom` se genera cuando termina el tema de la clase. El de `clase_11` cierra con una sección "Para revisar de forma autónoma" (cálculos del MIPS R4000 que quedaron en las diapositivas sin dictarse): es el patrón para contenido de las diapositivas que se deja a los estudiantes. Cuando una sesión se reparte entre dos clases, su fila del `README.md` raíz enlaza ambas (p. ej. la del 24/09: simulador de paginación + inicio de TLB). Cuando una sesión cubre el final de un tema y el inicio del siguiente (p. ej. la introducción a paginación del 15/09), el `apuntes_zoom` de la clase que termina solo lo resume como puente hacia la siguiente, y el de la clase siguiente lo desarrolla completo (p. ej. `clase_10` arranca con esa introducción del 15/09 y termina con un puente hacia la TLB).
- En algunas clases el código/simulador vive además en una carpeta hermana (`simulacion/`, `simulador/`), fuera de `apuntes/`.
- `clase_12` (*Beyond Physical Memory: Mechanisms*, swapping) y `clase_13` (*Beyond Physical Memory: Policies*, reemplazo de páginas) por ahora solo tienen `.pptx`/`.pdf` y su carpeta `simulador/`: no tienen `apuntes/`, `apuntes_zoom/` ni `_annotated.pdf`, y aún no están enlazadas en el `README.md` raíz.

Dentro de `clase_07/apuntes/` (`address_spaces/`), `clase_08/apuntes/` (`address_translation_base-bound/`), `clase_09/apuntes/` (`segmentation/`), `clase_10/apuntes/` (`intro-to-paging/`) y `clase_11/apuntes/` (`translation-lookaside-buffers/`) hay subtemas, cada uno con su propio `README.md`, su `img/` y, cuando corresponde, una carpeta `lab/` o `src/` con el enunciado del laboratorio y su código fuente. Aunque `clase_07` a `clase_11` tienen subtemas, su `apuntes_zoom/README.md` sigue siendo único y directo bajo `apuntes_zoom/` (no dividido por subtema); el patrón `apuntes_zoom/<subtema>/README.md` descrito para clases con subtemas aún no se ha usado en la práctica.

## Código de ejemplo y simuladores por clase

- **`clase_02/apuntes/code/`** — Programas en C (`cpu.c`, `threads.c`, `io.c`, `mem.c`) que ilustran CPU, hilos, I/O y memoria. Se compilan con el `Makefile` local:
  ```bash
  make            # compila todos (cpu, threads, io, mem)
  make cpu        # compila uno solo (o threads/io/mem)
  make clean      # elimina los ejecutables
  ```
  `threads` requiere `-pthread` (ya incluido en el Makefile). Antes de ejecutar, revisar la advertencia del propio README sobre las recomendaciones de compilación del repo original de OSTEP.

- **`clase_03/apuntes/` y `clase_03/simulacion/`** — Ambas carpetas contienen el mismo simulador `process-run.py` (tomado del homework `cpu-intro` de OSTEP) para observar el ciclo de vida de un proceso (uso de CPU vs. I/O):
  ```bash
  python3 process-run.py -l 5:100,5:100 -c -p
  ```

- **`clase_05/simulador/scheduler.py`** — Simulador de políticas de planificación (FIFO, SJF, RR) del homework `cpu-sched` de OSTEP:
  ```bash
  python3 scheduler.py -l 200,200,200 -p FIFO
  ```

- **`clase_06/simulador/mlfq.py`** — Simulador de *Multi-Level Feedback Queue* (homework `cpu-sched-mlfq` de OSTEP).

- **`clase_07/apuntes/address_spaces/lab/`** — Laboratorio de espacios de direcciones: `use-memory.c` (ya compilado/no se debe invocar directo) y `monitor-memory.sh`, que envuelve `use-memory` con `time` de GNU:
  ```bash
  bash monitor-memory.sh 200
  ```
  También contiene `vm-intro/` con su propio `Makefile` (`make`, `make clean`) para `virtual_address.c` (objetivo `va`).

- **`clase_08/apuntes/address_translation_base-bound/lab/`** — Enunciado del laboratorio de traducción de direcciones con registros base/límite (el simulador que referencia vive en `clase_08/simulador/`, no dentro de esta carpeta).

- **`clase_08/simulador/relocation.py`** — Simulador de *dynamic relocation* con registros base y límite (homework `vm-mechanism` de OSTEP), referenciado desde el laboratorio anterior:
  ```bash
  python3 relocation.py -s 1 -n 10 -l 100
  ```

- **`clase_09/apuntes/segmentation/src/seg.c`** — Fragmento de pseudocódigo (no compilable, sin `Makefile`) que ilustra la traducción de direcciones con segmentación.

- **`clase_10/apuntes/intro-to-paging/src/paging.c`** — Igual que el anterior, pseudocódigo no compilable (sin `Makefile`) de la traducción de direcciones con paginación; también aparece embebido en su `README.md`.

- **`clase_09/simulador/segmentation.py`** — Simulador de segmentación (homework `vm-segmentation` de OSTEP):
  ```bash
  python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20
  ```

- **`clase_10/simulador/paging-linear-translate.py`** — Simulador de traducción con tablas de página lineales (homework `vm-paging` de OSTEP); su `README.md` trae las preguntas de la tarea, que usan las banderas `-P`, `-a`, `-p`, `-v`, `-u`, `-n`, `-s`, `-c`:
  ```bash
  python3 paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 50
  ```

- **`clase_11/simulacion/`** — Práctica guiada de medición empírica de la TLB (método de Saavedra-Barrera, homework de fin del cap. 19 de OSTEP). No es una copia de un homework de OSTEP sino material propio generado con IA:
  - `tlb_bench.c`: micro-benchmark (C11, `-O2 -Wall -Wextra`) que recorre un arreglo tocando un entero por página y cronometra con `clock_gettime`; fija el proceso a un CPU con `sched_setaffinity` (específico de Linux). Sus comentarios `[R1]`–`[R7]` mapean las 7 preguntas del homework original: son documentación interna para el docente y deben conservarse.
  - `plot_tlb.py`: grafica ns/acceso vs. páginas (escala log2) y anota los escalones; **requiere `matplotlib`** (excepción a la regla de "sin dependencias").
  - `Makefile`: parámetros configurables como variables (`PAGINAS_MIN`, `PAGINAS_MAX`, `REPETICIONES`, `CPU`, `ACCESOS_OBJETIVO`):
    ```bash
    make            # compila tlb_bench
    make run        # genera resultados.csv
    make plot       # genera resultados.png (corre run si hace falta)
    make clean
    make run PAGINAS_MAX=16384 CPU=-1   # ejemplo: menos RAM, sin pinning
    ```
  - `README.md`: guía dirigida al estudiante, autocontenida; la atribución a Saavedra-Barrera/OSTEP vive **solo** en su sección final "Referencias" (no narrar la procedencia en el cuerpo).
  - `ABOUT.md`: nota interna para el docente (no para estudiantes) con el historial de decisiones de diseño de la práctica; consultarla antes de modificar cualquiera de estos archivos. Está en `.gitignore` y existe solo en la copia local: no debe subirse al remoto.

- **`clase_12/simulador/`** — Tarea de medición con `vmstat` (homework `vm-beyondphys` de OSTEP): `mem.c` reserva un arreglo de N MB y lo recorre indefinidamente (se detiene con control-c); requiere Linux con `gcc`, `make` y `vmstat`. Su `README.md` trae las preguntas traducidas:
  ```bash
  make            # gcc -o mem mem.c -Wall -O
  ./mem 1         # y en otra terminal: vmstat 1
  ```

- **`clase_13/simulador/paging-policy.py`** — Simulador de políticas de reemplazo de páginas (`FIFO`, `LRU`, `OPT`, `UNOPT`, `RAND`, `CLOCK`) del homework `vm-beyondphys-policy` de OSTEP; banderas `-a`, `-f`, `-n`, `-p`, `-b`, `-C`, `-m`, `-s`, `-N`, `-c`. Su `README.md` (mismo formato que el de `clase_10/simulador/`) trae las 5 preguntas del cap. 22; la pregunta 2 menciona `MRU`, política que el simulador no implementa:
  ```bash
  python3 paging-policy.py -s 0 -n 10 -p LRU -C 3 -c
  ```

No hay build system, linter ni suite de pruebas a nivel de repositorio: cada script/Makefile es independiente y se ejecuta/compila desde su propia carpeta como se indica arriba. Los programas en C se compilan con `gcc` (con `-Wall`, y `-pthread` cuando usan hilos); los scripts en Python (Python 3) no tienen dependencias externas — se ejecutan directo con `python3` — salvo `clase_11/simulacion/plot_tlb.py`, que necesita `matplotlib`.

## Convenciones de contenido

- Los apuntes y los README de laboratorio están escritos en **español**; mantener ese idioma al editar o generar contenido nuevo en este repositorio.
- Los simuladores Python bajo `simulacion/`/`simulador/`/`lab/` son copias adaptadas de los homeworks de [OSTEP](https://github.com/remzi-arpacidusseau/ostep-homework) — al modificarlos, preservar la interfaz de línea de comandos (flags como `-l`, `-c`, `-p`, `-S`, `-I`, `-B`) ya que las preguntas de cada README hacen referencia directa a esas banderas.
- Las imágenes/GIFs referenciadas desde los `README.md` de `apuntes/` están en subcarpetas `img/` o `images/` junto al README; al añadir una nota nueva, seguir ese mismo patrón de ubicación relativa. Dentro de `apuntes_zoom/` la convención es únicamente `img/` (ver `prompt_maestro_apuntes_clase_so_v1.2.md`).
- Al generar o actualizar un `README.md` de `apuntes_zoom/`, seguir siempre `prompt_maestro_apuntes_clase_so_v1.2.md`. Su prioridad es que **lo que estudian los estudiantes no tenga errores**: el manuscrito anotado es la fuente primaria y el material de Zoom es secundario, no se inventan ejemplos, pero los errores evidentes del manuscrito se **corrigen** (no se propagan) y se le listan al profesor en la fase de cruce de fuentes; solo los comandos y salidas de simulador efectivamente ejecutados se dejan tal cual, con nota si hace falta. Toda cifra, conversión y enlace del borrador se recalcula/verifica antes de entregarlo. Los insumos de Zoom se reciben uno a uno hasta el aviso "material de Zoom listo", y solo entonces se cruzan las fuentes.
