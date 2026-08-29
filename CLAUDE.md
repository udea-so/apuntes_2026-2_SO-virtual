# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Qué es este repositorio

Apuntes de clase para el curso de **Sistemas Operativos (Ude@)**, semestre 2026-2. No es un proyecto de software: es una colección de notas de clase, diapositivas y pequeños programas de ejemplo/laboratorio en C y Python usados para ilustrar conceptos del curso (basados en el libro *Operating Systems: Three Easy Pieces* / OSTEP). El `README.md` de la raíz es la tabla de contenidos del curso: enlaza cada clase con su PDF anotado, su carpeta de `apuntes` y, cuando existe, su `apuntes_zoom`.

## Estructura

Cada `clase_NN/` corresponde a una sesión de clase y sigue (aproximadamente) este patrón:

- `SO_apuntes_claseNN.pptx` / `.pdf` / `.xopp` / `_annotated.pdf`: diapositivas y anotaciones manuscritas (Xournal++) de la clase. Archivos binarios, no se editan con Claude.
- `apuntes/`: notas de clase en Markdown, con sus imágenes/GIFs de apoyo y, cuando aplica, el código o simulador asociado a esa clase. Es material teórico curado manualmente por el profesor.
- `apuntes_zoom/`: carpeta hermana de `apuntes/`, dedicada exclusivamente al resumen de la sesión generado combinando el manuscrito anotado con el/los resumen(es) de Zoom de la clase. Se genera siguiendo `prompt_maestro_apuntes_clase_so_v1.1.md` (raíz del repo) — ver esa guía para la plantilla, reglas de fidelidad a la fuente y convención de usar diagramas Mermaid donde ayuden a la pedagogía. No duplica código ni simuladores: solo referencia los ya existentes en `apuntes/` o en la carpeta hermana correspondiente. Existe (retroactivamente, un único `README.md` por carpeta) para `clase_02` a `clase_06`; algunas clases se dictaron en más de un día calendario y el propio `apuntes_zoom/README.md` señala con una nota el corte de sesión (p. ej. `clase_04` combina las sesiones del 13/08 y 18/08).
- En algunas clases el código/simulador vive además en una carpeta hermana (`simulacion/`, `simulador/`), fuera de `apuntes/`.

Dentro de `clase_07/apuntes/` hay varios subtemas (`address_spaces/`, `address_translation_base-bound/`), cada uno con su propio `README.md` y, cuando corresponde, una carpeta `lab/` con el enunciado del laboratorio y su código fuente. `apuntes_zoom/` sigue el mismo patrón de subtemas cuando aplique (`apuntes_zoom/<subtema>/README.md`); en las clases 2 a 6 (sin subtemas) es un único `README.md` directamente bajo `apuntes_zoom/`.

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

- **`clase_07/apuntes/address_translation_base-bound/lab/`** — Simulador de traducción de direcciones con registros base/límite:
  ```bash
  python3 mem_relocation.py 1
  ```

No hay build system, linter ni suite de pruebas a nivel de repositorio: cada script/Makefile es independiente y se ejecuta/compila desde su propia carpeta como se indica arriba. Los programas en C se compilan con `gcc` (con `-Wall`, y `-pthread` cuando usan hilos); los scripts en Python (Python 3) no tienen dependencias externas — se ejecutan directo con `python3`.

## Convenciones de contenido

- Los apuntes y los README de laboratorio están escritos en **español**; mantener ese idioma al editar o generar contenido nuevo en este repositorio.
- Los simuladores Python bajo `simulacion/`/`simulador/`/`lab/` son copias adaptadas de los homeworks de [OSTEP](https://github.com/remzi-arpacidusseau/ostep-homework) — al modificarlos, preservar la interfaz de línea de comandos (flags como `-l`, `-c`, `-p`, `-S`, `-I`, `-B`) ya que las preguntas de cada README hacen referencia directa a esas banderas.
- Las imágenes/GIFs referenciadas desde los `README.md` de `apuntes/` están en subcarpetas `img/` o `images/` junto al README; al añadir una nota nueva, seguir ese mismo patrón de ubicación relativa. Dentro de `apuntes_zoom/` la convención es únicamente `img/` (ver `prompt_maestro_apuntes_clase_so_v1.1.md`).
- Al generar o actualizar un `README.md` de `apuntes_zoom/`, seguir siempre `prompt_maestro_apuntes_clase_so_v1.1.md`: el manuscrito anotado es la fuente primaria, el resumen de Zoom es secundario, no se inventan ejemplos ni se corrigen silenciosamente inconsistencias del manuscrito, y las discrepancias relevantes entre fuentes se señalan al profesor antes de resolverlas.
