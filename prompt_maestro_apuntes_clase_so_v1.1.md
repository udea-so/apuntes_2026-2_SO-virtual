# Prompt Maestro — Apuntes de Clase (Repositorio `apuntes_2026-2_SO-virtual`)

**Versión**: 1.1
**Curso**: Sistemas Operativos — Universidad de Antioquia — Ingeniería de Sistemas (Ude@)
**Propósito**: Generar el archivo `README.md` de `apuntes_zoom/` (o de cada subtema de `apuntes_zoom/`, ver Sección 2) dentro de `clase_NN/` en este repositorio, combinando el manuscrito anotado (diapositivas rayadas en Xournal++) con el resumen de Zoom de la sesión, para que un estudiante que no asistió sepa exactamente qué se dictó y pueda repasarlo.

> **Registro de cambios v1.0 → v1.1**: el documento generado ya no vive dentro de la carpeta `apuntes/` existente de cada clase (que sigue siendo el material teórico curado manualmente, con o sin código/simulador asociado); pasa a un directorio hermano nuevo, `clase_NN/apuntes_zoom/`, dedicado exclusivamente al resumen combinado manuscrito + Zoom. Se refuerza además el uso de diagramas Mermaid (u otra sintaxis renderizable nativamente por GitHub) donde ayuden a la comprensión, no solo cuando el manuscrito ya trae un diagrama equivalente dibujado a mano.

**Adaptado de**: `prompt_maestro_apuntes_clase_v1.3.md` (curso Matemáticas Discretas 1, repositorio `apuntes_clases_20XX-X`). La diferencia estructural principal es que **este repositorio no tiene un sitio GitHub Pages paralelo con el contenido de estudio**: la carpeta `apuntes/` de cada clase ya cumple ese rol de material teórico curado. `apuntes_zoom/` es, entonces, el equivalente al "resumen de bitácora por clase" de Discretas 1, pero con una diferencia clave: aquí también incorpora el resumen teórico expandido del tema (no solo agenda/pendientes), porque no existe otro documento que lo haga — por eso "Contenido temático" es una sección **obligatoria**, no condicional. La referencia canónica de contenido, cuando no hay `apuntes/` propio para la clase o para profundizar, es el libro *Operating Systems: Three Easy Pieces* / OSTEP, citado por capítulo cuando aplique.

---

## 1. Insumos de entrada

Para cada clase, el material de entrada puede incluir cualquier combinación de:

1. **Manuscrito anotado** (`SO_apuntes_claseNN_annotated.pdf`, generado desde `.xopp` sobre la diapositiva rayada) — **fuente primaria, obligatoria si existe**.
2. **Resumen generado por Zoom** (transcripción/resumen automático de la sesión grabada) — fuente secundaria. Si la clase se dictó en más de un día calendario (como ya ha ocurrido: Clase 4/5 el 13/08 y Clase 5/6 el 18-20/08 según la tabla del `README.md` raíz), puede haber un resumen de Zoom por sesión; se cruzan entre sí y con el manuscrito para resolver dudas sobre lo efectivamente dictado.
3. **Diapositivas originales** (`SO_apuntes_claseNN.pptx` / `.pdf`) — referencia de apoyo para el orden pensado de la clase, no necesariamente el orden real dictado (ver Sección 3).
4. **Material adicional**: capturas, diagramas (`.drawio`/`.png`), fragmentos de código o del simulador de la clase efectivamente ejecutados/mostrados, mensajes de foro relacionados.

Claude debe recibir estos insumos y **esperar instrucción explícita** antes de generar o modificar el `README.md`, salvo que el profesor indique lo contrario (ver Fase 1).

---

## 2. Ubicación y estructura fija del documento

### Dónde va el archivo

- Caso general: `clase_NN/apuntes_zoom/README.md` — carpeta nueva, hermana de `apuntes/` (y de `simulador/`/`simulacion/` cuando existan) dentro de `clase_NN/`. No se mezcla con el contenido de `apuntes/`; ambas carpetas coexisten y cada una conserva su propósito (`apuntes/` = material teórico curado manualmente por el profesor, con o sin código; `apuntes_zoom/` = resumen combinado manuscrito + Zoom de la sesión, generado con este prompt maestro).
- Caso de clase con varios subtemas independientes (siguiendo el precedente de `clase_07/apuntes/`, que separa `address_spaces/` y `address_translation_base-bound/`): un `README.md` por subtema, en `clase_NN/apuntes_zoom/<subtema>/README.md`. Se usa este patrón solo cuando los subtemas son lo bastante distintos como para merecer una sección independiente en la tabla del `README.md` raíz.
- Imágenes/GIFs de apoyo van en una subcarpeta `img/` hermana del `README.md` que las usa (`clase_NN/apuntes_zoom/img/`, o `clase_NN/apuntes_zoom/<subtema>/img/`). Se usa `img/` como convención única dentro de `apuntes_zoom/` (a diferencia de `apuntes/`, donde hoy conviven `img/` e `images/` según la clase — ver Sección 6 de la versión anterior de este documento). Se referencian con ruta relativa (`img/nombre.png`, no ruta absoluta).
- `apuntes_zoom/` no aloja código ni simuladores propios: si la clase usa uno del curso (`process-run.py`, `scheduler.py`, `mlfq.py`, etc., o el de `apuntes/<subtema>/lab/` cuando exista), el apunte solo referencia los comandos/banderas mostrados, sin duplicar el código fuente.

### Plantilla

```markdown
![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# [Título del tema — no "Clase N", ver nota]

## Objetivos de Aprendizaje

* **[Verbo]**: [objetivo 1, formulado como lo que el estudiante logra comprender/hacer]
* **[Verbo]**: [objetivo 2]
* ...

## [Sección temática 1]

[Explicación en prosa/bullets, con diagramas, fórmulas, fragmentos de código o
comandos del simulador/laboratorio efectivamente mostrados en la sesión.]

## [Sección temática 2]

...

## Resumen y Conclusión (condicional — clases con cierre de un ciclo temático completo, como fin de módulo)

[Síntesis del tema, tabla comparativa si aplica, y qué pregunta o tema abre la siguiente clase.]

---

> [!IMPORTANT]
> **Nota de Transparencia:** Este documento fue generado y adaptado mediante el uso de **IA Generativa**, a partir del manuscrito anotado de la clase[ y el resumen de la sesión de Zoom]. El contenido ha sido supervisado, validado y refinado por intervención humana para garantizar su precisión técnica y coherencia pedagógica. No obstante, pueden haber errores.
```

### Reglas de estructura

- **Badge de encabezado**: `![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)` es fijo, primera línea del documento, en todo `README.md` generado o adaptado con ayuda de IA — ya es la convención observada en `clase_05/apuntes/README.md`. Clases antiguas sin badge (p. ej. `clase_04`) no se editan retroactivamente solo para añadirlo (ver Fase 5).
- **Nota de Transparencia** al final del documento es igualmente fija cuando el contenido se generó/adaptó con IA — es la convención ya usada en `clase_05/apuntes/README.md`. Ambas (badge + nota) son redundantes a propósito: el badge se ve al navegar la carpeta, la nota da el detalle al leer el documento completo.
- **Título**: nombra el tema (p. ej. "Planificación de CPU (CPU Scheduling)"), no "Clase N — ...". El número y fecha de la clase ya están cubiertos por la tabla del `README.md` raíz del repositorio (que enlaza PDF anotado + carpeta `apuntes/` y, cuando exista, `apuntes_zoom/`); no se duplica esa metadata dentro del documento.
- **Objetivos de Aprendizaje** son 3-5 bullets con verbo en infinitivo o imperativo de acción cognitiva (**Identificar**, **Explicar**, **Comparar**, **Aplicar**, **Medir**...), seguidos de dos puntos y la descripción — siguiendo el formato ya usado en `clase_05/apuntes/README.md`. No son una lista de subtítulos del documento ni una copia de la agenda de la diapositiva.
- **Secciones temáticas**: cada tema efectivamente cubierto en la sesión es un `##` (o `###` si es subtema dentro de un tema mayor, como los distintos algoritmos de planificación dentro de "Políticas de Planificación"). El orden de las secciones sigue el **orden real en que se dictó la clase según el manuscrito**, no el orden idealizado de la diapositiva de agenda cuando estos difieren (ver Sección 3).
  - **Clases dictadas en más de un día calendario**: si cada sesión cubrió temas claramente distintos, puede anotarse el corte con una nota breve al inicio de la sección correspondiente (p. ej. "Continuación de la sesión del 13/08"), sin necesidad de una sección de "Agenda" separada por sesión — a diferencia del curso de Discretas, aquí no se mantiene una sección de Agenda aparte del contenido mismo.
- **Diagramas, fórmulas y código**: cualquier GIF/imagen, fórmula (LaTeX inline con `$...$` o en bloque `$$...$$`, ya usado en `clase_05`), fragmento de código o comando de simulador incluido debe corresponder a algo **efectivamente mostrado o trabajado en la sesión** (según el manuscrito y/o el resumen de Zoom) — nunca un ejemplo inventado o "lo esperable" para el tema, aunque sea pedagógicamente razonable.
  - Cuando la clase usa uno de los simuladores/laboratorios del curso (`process-run.py`, `scheduler.py`, `mlfq.py`, `mem_relocation.py`, los programas en C de `clase_02`, etc.), los comandos y banderas citados en el apunte deben coincidir exactamente con los mostrados en clase, preservando las banderas (`-l`, `-c`, `-p`, `-S`, `-I`, `-B`, etc.) tal como se documentan en el `CLAUDE.md` del repositorio.
  - Recuadros de énfasis (`> [!Note]`, `> [!Tip]`, `> [!Important]`) en formato nativo de GitHub, como ya se usa en `clase_05` — este repositorio es Markdown plano leído directamente en GitHub, no pasa por Jekyll.
- **Diagramas renderizables por GitHub**: además de las imágenes/GIFs tomados o adaptados del manuscrito, se debe añadir un diagrama en Mermaid (` ```mermaid `) **siempre que un concepto se preste a representación visual y aún no tenga una imagen equivalente clara** — no solo cuando el manuscrito ya trae uno dibujado a mano. GitHub renderiza Mermaid de forma nativa en cualquier `README.md`, así que es la herramienta por defecto para mejorar la pedagogía del apunte sin depender de capturas externas. Casos típicos en este curso:
  - `graph`/`flowchart`: orden de llegada a una cola, decisiones de un planificador, protocolo de arranque/trap.
  - `sequenceDiagram`: interacción entre proceso, hardware y SO (p. ej. trap → modo kernel → return-from-trap; interrupción de temporizador → context switch).
  - `stateDiagram-v2`: transiciones entre estados de un proceso (Listo/Ejecutando/Bloqueado/Terminado) o de una cola en MLFQ.
  - `gantt` o `graph LR` con nodos por instante de tiempo: línea de tiempo de ejecución de procesos (como ya se hace con las tablas de turnaround/response time en `clase_05`).
  Si Mermaid no expresa bien el concepto (p. ej. un layout de memoria o un mapa de bits), se prioriza una imagen/captura del manuscrito antes que forzar un diagrama Mermaid poco claro. En cualquier caso, el diagrama debe representar fielmente lo mostrado o explicado en clase (ver Sección 3), no un ejemplo genérico del tema.
- **Resumen y Conclusión** es condicional: solo cuando la clase cierra un ciclo temático completo (p. ej. el final del bloque de políticas de planificación) y amerita una tabla comparativa o síntesis. Una clase que introduce un tema nuevo sin cerrarlo aún no necesita esta sección.
- **Referencia a OSTEP**: cuando el tema de la clase corresponde directamente a un capítulo del libro *Operating Systems: Three Easy Pieces*, puede mencionarse el capítulo entre paréntesis la primera vez que se nombra el tema (p. ej. "Ejecución Directa Limitada (OSTEP, cap. 6)"). No es obligatorio ni se fuerza si el manuscrito no lo menciona explícitamente.

---

## 3. Reglas de fidelidad a la fuente

- Toda cifra, fórmula, nombre de algoritmo, comando o resultado de ejemplo debe verificarse contra el **manuscrito anotado** como fuente primaria. El resumen de Zoom es secundario/complementario — útil para contexto conversacional (preguntas de estudiantes, aclaraciones dichas en voz alta que no quedaron escritas, pendientes) pero no para datos duros si hay conflicto con el manuscrito.
- Si el manuscrito y el resumen de Zoom se contradicen en algún dato relevante para el estudiante (una fórmula, un resultado numérico de un ejemplo, el nombre de un algoritmo), señalar la discrepancia explícitamente al profesor antes de resolverla unilateralmente — nunca elegir silenciosamente una versión.
- Discrepancias que son evidentemente errores de transcripción automática de Zoom sin relevancia pedagógica (un símbolo mal transcrito cuando el manuscrito lo muestra con claridad) se resuelven usando el manuscrito como fuente de verdad, sin necesidad de nota visible.
- Si el propio manuscrito contiene una inconsistencia interna menor (p. ej. un cálculo con un valor que no cuadra con la fórmula mostrada un renglón antes), se transcribe tal como aparece y se señala con una nota aclaratoria breve, en vez de "corregirla" silenciosamente — alterar el contenido de la fuente no es fidelidad, aunque el error parezca menor y fácil de arreglar.
- El orden de las secciones temáticas sigue el orden real de dictado (manuscrito), que puede no coincidir con el orden de la diapositiva de agenda original — la diapositiva refleja el plan, el manuscrito refleja lo que realmente ocurrió.
- No inventar ni completar ejemplos, resultados de simulador o pasos de un cálculo que no fueron mostrados en clase, aunque sea "lo esperable" para el tema.

---

## 4. Flujo de trabajo (fases)

**Fase 1 — Recepción de material**
Claude recibe el/los archivo(s) (manuscrito anotado, resumen(es) de Zoom, diapositivas, otros apuntes) y confirma qué insumos tiene disponibles para esa clase. No genera nada todavía — espera instrucción explícita del profesor para proceder.

**Fase 2 — Extracción y verificación**
Claude extrae la información relevante de cada fuente, aplicando las reglas de la Sección 3. Si detecta discrepancias relevantes entre fuentes, o no tiene claro si una sección amerita incluirse (p. ej. "Resumen y Conclusión"), lo señala antes de redactar.

**Fase 3 — Borrador**
Claude genera el `README.md` completo siguiendo la plantilla de la Sección 2, ubicado en la ruta correcta (`clase_NN/apuntes_zoom/README.md` o `clase_NN/apuntes_zoom/<subtema>/README.md`), e incluye las imágenes referenciadas como archivos hermanos en `img/` y los diagramas Mermaid inline en el propio Markdown.

**Fase 4 — Revisión y ajuste**
El profesor revisa y solicita ajustes de forma (tono, tablas vs. listas, reordenamiento) o de fondo (correcciones de contenido). Los ajustes se aplican en rondas, no uno a uno sin confirmación previa cuando son varios.

**Fase 5 — Cierre**
Una vez aprobado, el archivo queda como definitivo para esa clase y se actualiza la fila correspondiente en el `README.md` raíz (tabla de "Apuntes por clase") agregando el enlace a `apuntes_zoom/` junto al ya existente hacia `apuntes/` (cuando ambas carpetas existan para esa clase). Clases anteriores ya publicadas no se reformatean retroactivamente solo para ajustarse a una versión más nueva de este prompt maestro salvo que el profesor lo pida explícitamente — cada `README.md` es un registro fiel de lo dictado, generado con las convenciones vigentes en su momento.

---

## 5. Convenciones heredadas del repositorio

- Los apuntes están escritos en **español** (ver `CLAUDE.md` del repositorio).
- Tablas en Markdown estándar (`|---|---|`); LaTeX inline (`$...$`, `$$...$$`) para fórmulas, ya usado en `clase_05/apuntes/`.
- **Estructura de archivos por clase**: `clase_NN/` contiene las diapositivas y anotaciones (`.pptx`, `.pdf`, `.xopp`, `_annotated.pdf`) como hermanos de `apuntes/` y, a partir de esta versión, de `apuntes_zoom/`. Dentro de `apuntes_zoom/` van el/los `README.md` y su carpeta `img/`; el código o simulador de la clase sigue viviendo únicamente bajo `apuntes/` o en su carpeta hermana `simulador/`/`simulacion/` (ver `CLAUDE.md` para el detalle de cada simulador y sus banderas CLI) — no se duplica dentro de `apuntes_zoom/`.
- Los simuladores Python bajo `simulacion/`/`simulador/`/`lab/` son copias adaptadas de los homeworks de OSTEP — al citarlos en el apunte, preservar la interfaz de línea de comandos documentada en `CLAUDE.md`.

---

## 6. Preguntas abiertas

- ¿Las clases ya publicadas sin `apuntes_zoom/` (`clase_02` a `clase_07`, generadas antes de esta versión) se completan retroactivamente con este formato, o `apuntes_zoom/` arranca solo desde la próxima clase dictada? (Este documento no asume una respuesta; se debe confirmar con el profesor antes de generar el primer `apuntes_zoom/`.)
- ¿Las clases ya publicadas sin badge/objetivos en `apuntes/` (`clase_02`, `clase_03`, `clase_04`) se migran alguna vez a ese formato, o quedan como están por ser registro histórico? (Sección 4, Fase 5, asume que quedan como están salvo pedido explícito — aplica solo a `apuntes/`, no a `apuntes_zoom/`, que es contenido nuevo.)
