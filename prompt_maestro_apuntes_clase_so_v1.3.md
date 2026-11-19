# Prompt Maestro — Apuntes de Clase (Repositorio `apuntes_2026-2_SO-virtual`)

**Versión**: 1.3
**Curso**: Sistemas Operativos — Universidad de Antioquia — Ingeniería de Sistemas (Ude@)
**Propósito**: Generar el archivo `README.md` de `apuntes_zoom/` (o de cada subtema de `apuntes_zoom/`, ver Sección 2) dentro de `clase_NN/` en este repositorio, combinando el manuscrito anotado (diapositivas rayadas en Xournal++) con el resumen de Zoom de la sesión, para que un estudiante que no asistió sepa exactamente qué se dictó y pueda repasarlo. Desde la v1.3 cubre también la revisión y documentación de la carpeta `clase_NN/ejemplos/` (Sección 7), que es insumo del apunte en las clases del módulo 3.

> **Registro de cambios v1.0 → v1.1**: el documento generado ya no vive dentro de la carpeta `apuntes/` existente de cada clase (que sigue siendo el material teórico curado manualmente, con o sin código/simulador asociado); pasa a un directorio hermano nuevo, `clase_NN/apuntes_zoom/`, dedicado exclusivamente al resumen combinado manuscrito + Zoom. Se refuerza además el uso de diagramas Mermaid (u otra sintaxis renderizable nativamente por GitHub) donde ayuden a la comprensión, no solo cuando el manuscrito ya trae un diagrama equivalente dibujado a mano.

> **Registro de cambios v1.1 → v1.2** (a partir de la generación de `clase_09/apuntes_zoom/`): el objetivo prioritario pasa a ser que **lo que revisan los estudiantes no tenga errores**. En consecuencia: (1) los errores evidentes del manuscrito se **corrigen** en vez de transcribirse con nota (Sección 3), salvo comandos y salidas de simulador efectivamente ejecutados; (2) toda cifra, conversión y enlace del borrador se verifica antes de entregarlo (Sección 3 y Fase 3); (3) los insumos de Zoom se reciben de forma iterativa, uno por mensaje, hasta que el profesor avise "material de Zoom listo" (Fase 1); (4) se añade la regla de "puente" para sesiones que cierran un tema y abren el siguiente (Sección 2); (5) los elementos administrativos individuales no se incluyen (Sección 2); (6) la Fase 5 incluye verificar los enlaces del `README.md` raíz y actualizar `CLAUDE.md`.

> **Registro de cambios v1.2 → v1.3** (a partir de `clase_16` y `clase_17`, módulo 3): desde la clase 14 el código de cada clase vive en `clase_NN/ejemplos/`, con guías y salidas de referencia, y las clases que faltan seguirán ese patrón. En consecuencia: (1) la carpeta `ejemplos/` es un **insumo** del apunte (Sección 1) y cada sección del apunte enlaza el programa y la sección de la guía que implementan sus diapositivas, con un bloque "Cómo verlo" (fragmento clave, comando y salida; Sección 2); (2) toda salida citada declara su **origen**: ejecutada en la sesión, captura incluida en el deck, o ejecución de referencia con fecha y entorno (Sección 3); (3) se añade la **Fase 0** y la **Sección 7**, con el procedimiento para revisar, corregir, verificar en WSL y documentar la carpeta `ejemplos/` (como se hizo con `clase_17/ejemplos/` el 19/11/2026); (4) también se corrigen los **errores conceptuales** de las explicaciones del Zoom y del código de las diapositivas, no solo los del manuscrito (Sección 3); (5) los insumos de Zoom pueden llegar con la fecha equivocada o repetidos: se identifican contra las marcas del manuscrito (Fase 1); (6) la Fase 3 usa herramientas para verificar enlaces, anclas y diagramas Mermaid (Sección 4); (7) se recogen en la plantilla las secciones opcionales que se consolidaron entre `clase_11` y `clase_17` ("Punto de partida", recuadros "Complemento (OSTEP, cap. N)", "Para revisar de forma autónoma", "Puente hacia la siguiente clase", "Para probar por cuenta propia", "Referencias").

**Adaptado de**: `prompt_maestro_apuntes_clase_v1.3.md` (curso Matemáticas Discretas 1, repositorio `apuntes_clases_20XX-X`). La diferencia estructural principal es que **este repositorio no tiene un sitio GitHub Pages paralelo con el contenido de estudio**: la carpeta `apuntes/` de cada clase ya cumple ese rol de material teórico curado. `apuntes_zoom/` es, entonces, el equivalente al "resumen de bitácora por clase" de Discretas 1, pero con una diferencia clave: aquí también incorpora el resumen teórico expandido del tema (no solo agenda/pendientes), porque no existe otro documento que lo haga — por eso "Contenido temático" es una sección **obligatoria**, no condicional. La referencia canónica de contenido, cuando no hay `apuntes/` propio para la clase o para profundizar, es el libro *Operating Systems: Three Easy Pieces* / OSTEP, citado por capítulo cuando aplique.

---

## 1. Insumos de entrada

Para cada clase, el material de entrada puede incluir cualquier combinación de:

1. **Manuscrito anotado** (`SO_apuntes_claseNN_annotated.pdf`, generado desde `.xopp` sobre la diapositiva rayada) — **fuente primaria, obligatoria si existe**.
2. **Material de Zoom** — fuente secundaria. Por cada sesión puede haber dos tipos de insumo: el **resumen automático** generado por Zoom y las **notas de Zoom del profesor** (más detalladas, con citas). Ninguno de los dos es confiable para datos duros: ambos pueden traer cifras mal transcritas (p. ej. en `clase_09`, "PA = 32 068" por 32 868) o confundir datos de ejemplos distintos (el tamaño de un segmento del simulador atribuido al ejemplo de clase). Si la clase se dictó en más de un día calendario (como ya ha ocurrido: Clase 4/5 el 13/08 y Clase 5/6 el 18-20/08 según la tabla del `README.md` raíz), puede haber un resumen de Zoom por sesión; se cruzan entre sí y con el manuscrito para resolver dudas sobre lo efectivamente dictado.
3. **Diapositivas originales** (`SO_apuntes_claseNN.pptx` / `.pdf`) — referencia de apoyo para el orden pensado de la clase, no necesariamente el orden real dictado (ver Sección 3).
4. **Material adicional**: capturas, diagramas (`.drawio`/`.png`), fragmentos de código o del simulador de la clase efectivamente ejecutados/mostrados, mensajes de foro relacionados.
5. **Material ya existente en el repositorio**: la carpeta `apuntes/` de la clase (y su `src/` o `lab/`) y el código del simulador en `simulador/`/`simulacion/`. Sirven para verificar banderas, fórmulas y resultados, y para enlazarlos desde el apunte (sin duplicarlos).
6. **Carpeta `ejemplos/` de la clase** (módulo 3 en adelante; precedentes: `clase_14` a `clase_17`): los programas que implementan las diapositivas, con un `README.md` índice, una guía por carpeta y salidas de una ejecución de referencia. Antes de usarla como insumo debe estar revisada y verificada según la Sección 7 (Fase 0). Sirve para: (a) relacionar cada página de las diapositivas con su programa (tabla "Relación con las diapositivas y el libro" de cada guía); (b) tomar fragmentos de código que **compilan**, en lugar de transcribir el de la diapositiva con sus errores; (c) tomar salidas reales para los bloques "Cómo verlo"; (d) enlazar, desde el apunte, el `.c` y la sección exacta de la guía. Las capturas de terminal que aparecen en las diapositivas también son insumo, pero hay que establecer si vienen en el deck original o se tomaron en la sesión: `pdfimages -list` sobre el `.pdf` de las diapositivas muestra las imágenes del deck, y el `.xopp` (XML comprimido con gzip) muestra si en una página se pegó una imagen durante la clase (`<image>`).

Claude debe recibir estos insumos y **esperar instrucción explícita** antes de generar o modificar el `README.md`, salvo que el profesor indique lo contrario (ver Fase 1).

---

## 2. Ubicación y estructura fija del documento

### Dónde va el archivo

- Caso general: `clase_NN/apuntes_zoom/README.md` — carpeta nueva, hermana de `apuntes/` (y de `simulador/`/`simulacion/` cuando existan) dentro de `clase_NN/`. No se mezcla con el contenido de `apuntes/`; ambas carpetas coexisten y cada una conserva su propósito (`apuntes/` = material teórico curado manualmente por el profesor, con o sin código; `apuntes_zoom/` = resumen combinado manuscrito + Zoom de la sesión, generado con este prompt maestro).
- Caso de clase con varios subtemas independientes (siguiendo el precedente de `clase_07/apuntes/`, que separa `address_spaces/` y `address_translation_base-bound/`): un `README.md` por subtema, en `clase_NN/apuntes_zoom/<subtema>/README.md`. Se usa este patrón solo cuando los subtemas son lo bastante distintos como para merecer una sección independiente en la tabla del `README.md` raíz.
- Imágenes/GIFs de apoyo van en una subcarpeta `img/` hermana del `README.md` que las usa (`clase_NN/apuntes_zoom/img/`, o `clase_NN/apuntes_zoom/<subtema>/img/`). Se usa `img/` como convención única dentro de `apuntes_zoom/` (a diferencia de `apuntes/`, donde hoy conviven `img/` e `images/` según la clase — ver Sección 6 de la versión anterior de este documento). Se referencian con ruta relativa (`img/nombre.png`, no ruta absoluta).
- `apuntes_zoom/` no aloja código ni simuladores propios: si la clase usa uno del curso (`process-run.py`, `scheduler.py`, `mlfq.py`, etc., o el de `apuntes/<subtema>/lab/` cuando exista), el apunte solo referencia los comandos/banderas mostrados, sin duplicar el código fuente.
  - **Excepción, solo a pedido del profesor** (precedente: `clase_14`): una subcarpeta `apuntes_zoom/ejemplos/` con copias de los programas efectivamente compilados y ejecutados en clase, cada carpeta con un `README.md` de repaso rápido (cómo compilar, cómo ejecutar y salida). Si no hay capturas de la sesión, las salidas se obtienen ejecutando el código y se indica explícitamente que son de una ejecución de referencia (fecha y entorno), no de la clase; un programa que solo se explicó se marca como no ejecutado en clase. No se suben ejecutables.

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

## Para revisar de forma autónoma (condicional)

## Puente hacia la siguiente clase (condicional)

## Para probar por cuenta propia (condicional)

## Referencias

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
  - **Clases dictadas en más de un día calendario**: si cada sesión cubrió temas claramente distintos, puede anotarse el corte con una nota breve al inicio de la sección correspondiente (p. ej. "Continuación: sesión del 15/09/2026"), sin necesidad de una sección de "Agenda" separada por sesión — a diferencia del curso de Discretas, aquí no se mantiene una sección de Agenda aparte del contenido mismo. El manuscrito suele marcar el corte a mano (p. ej. "Fin: 10/09/2026 · Inicio: 15/09/2026" en `clase_09`).
  - **Sesiones que cierran un tema y abren el siguiente (regla de "puente")**: si en la misma sesión se terminó el tema de la clase N y se empezó el de la clase N+1 (p. ej. el 15/09 se cerró segmentación y se introdujo paginación), el `apuntes_zoom` de la clase N cubre solo su tema y termina con una sección breve "Puente hacia la siguiente clase" que resume lo introducido del tema nuevo y las preguntas que lo guían. El desarrollo completo de ese contenido va en el `apuntes_zoom` de la clase N+1.
- **Elementos administrativos**: **no** se incluyen en el apunte, ni los del grupo (fechas de parciales y quices, actividades opcionales, entregas) ni los de un estudiante concreto. Su comunicación es responsabilidad del docente en los foros de Moodle en Ude@; el apunte es solo material de estudio. Por pedido del profesor, las notas de "Evaluaciones" que traían los `apuntes_zoom` de `clase_09` a `clase_12` se retiraron.
- **Diagramas, fórmulas y código**: cualquier GIF/imagen, fórmula (LaTeX inline con `$...$` o en bloque `$$...$$`, ya usado en `clase_05`), fragmento de código o comando de simulador incluido debe corresponder a algo **efectivamente mostrado o trabajado en la sesión** (según el manuscrito y/o el resumen de Zoom) — nunca un ejemplo inventado o "lo esperable" para el tema, aunque sea pedagógicamente razonable.
  - Cuando la clase usa uno de los simuladores/laboratorios del curso (`process-run.py`, `scheduler.py`, `mlfq.py`, `mem_relocation.py`, los programas en C de `clase_02`, etc.), los comandos y banderas citados en el apunte deben coincidir exactamente con los mostrados en clase, preservando las banderas (`-l`, `-c`, `-p`, `-S`, `-I`, `-B`, etc.) tal como se documentan en el `CLAUDE.md` del repositorio.
  - Recuadros de énfasis (`> [!Note]`, `> [!Tip]`, `> [!Important]`) en formato nativo de GitHub, como ya se usa en `clase_05` — este repositorio es Markdown plano leído directamente en GitHub, no pasa por Jekyll.
- **Diagramas renderizables por GitHub**: además de las imágenes/GIFs tomados o adaptados del manuscrito, se debe añadir un diagrama en Mermaid (` ```mermaid `) **siempre que un concepto se preste a representación visual y aún no tenga una imagen equivalente clara** — no solo cuando el manuscrito ya trae uno dibujado a mano. GitHub renderiza Mermaid de forma nativa en cualquier `README.md`, así que es la herramienta por defecto para mejorar la pedagogía del apunte sin depender de capturas externas. Casos típicos en este curso:
  - `graph`/`flowchart`: orden de llegada a una cola, decisiones de un planificador, protocolo de arranque/trap.
  - `sequenceDiagram`: interacción entre proceso, hardware y SO (p. ej. trap → modo kernel → return-from-trap; interrupción de temporizador → context switch).
  - `stateDiagram-v2`: transiciones entre estados de un proceso (Listo/Ejecutando/Bloqueado/Terminado) o de una cola en MLFQ.
  - `gantt` o `graph LR` con nodos por instante de tiempo: línea de tiempo de ejecución de procesos (como ya se hace con las tablas de turnaround/response time en `clase_05`).
  Si Mermaid no expresa bien el concepto (p. ej. un layout de memoria o un mapa de bits), se prioriza una imagen/captura del manuscrito antes que forzar un diagrama Mermaid poco claro. En cualquier caso, el diagrama debe representar fielmente lo mostrado o explicado en clase (ver Sección 3), no un ejemplo genérico del tema.
  - **Texto recortado en los nodos**: Mermaid calcula el ancho de cada cuadro con una fuente y el visor (vista previa de VS Code, GitHub) puede dibujar el texto con otra más ancha, de modo que se pierden los últimos caracteres de las etiquetas largas (p. ej. "Excepción: segmentation fa..."). Para evitarlo, en todo diagrama con cuadros (`graph`/`flowchart`, `stateDiagram-v2`):
    1. La primera línea dentro del bloque fija la fuente: `%%{init: {"themeVariables": {"fontFamily": "Arial, Helvetica, sans-serif"}}}%%`.
    2. Las etiquetas largas de nodos y de flechas se parten con `<br/>` en líneas cortas (unas 20-25 letras por línea), p. ej. `G["Excepción:<br/>segmentation fault"]`.
    3. Dentro de las etiquetas, `<` y `>` se escriben con los códigos de Mermaid `#lt;` y `#gt;` (o en palabras: "menor que", "por debajo de"), porque Mermaid puede interpretarlos como HTML (precedente: `clase_10/apuntes_zoom/README.md`, `PFN #lt;#lt; PFN_SHIFT`).
    Los `sequenceDiagram` no necesitan este ajuste, porque sus mensajes van sobre las flechas y no dentro de cuadros. Precedente: `clase_12/apuntes_zoom/README.md`.
- **Secciones opcionales consolidadas** (precedentes entre `clase_11` y `clase_17`):
  - **Punto de partida** (primera sección temática): cuando la sesión empieza con un repaso de la clase anterior, se resume en pocas líneas y se **enlaza** el apunte de esa clase (con ancla a la sección) en vez de repetirlo (`clase_16`, `clase_17`).
  - **Recuadros "Complemento (OSTEP, cap. N)"** (`> [!NOTE]`): contenido de OSTEP que no se dictó pero que hace falta para entender o corregir lo dictado (p. ej. la semántica de Mesa como base de la regla del `while` en `clase_17`). Se distinguen así de lo dictado.
  - **Para revisar de forma autónoma**: contenido que quedó en las diapositivas sin dictarse, que se dejó a los videos del Prof. Danny Múnera, o que completa el capítulo de OSTEP (p. ej. *covering conditions* en `clase_17`). Abre con un recuadro que dice por qué está ahí y con qué fuente se explica.
  - **Puente hacia la siguiente clase**: ver la regla de "puente" arriba.
  - **Para probar por cuenta propia**: enlaza las guías de `ejemplos/` (o la tarea del simulador) y la tarea (*Homework*) del capítulo de OSTEP, sin repetirlas.
  - **Referencias**: capítulos de OSTEP con versión (v1.10) y URL, artículos citados en los complementos y páginas de manual (`man ...`).
- **Código de la clase en `ejemplos/`** (clases con esa carpeta): cada sección temática que tenga un programa asociado cierra con un bloque **"Cómo verlo"** que contiene: el enlace al `.c` y a la sección de su guía (con ancla), el comando de las diapositivas tal cual (`gcc -I. ... -o nombre.out`) y, si difiere, el equivalente con `make` de la guía, y una salida con su origen declarado (Sección 3). El fragmento de código que se muestra en el apunte es el de las diapositivas **corregido** (el que compila en `ejemplos/`), no una transcripción con errores. El apunte no duplica las guías: muestra lo mínimo para entender la sesión y enlaza el resto (cómo forzar cada error, tablas de repeticiones, trazas).
- **Resumen y Conclusión** es condicional: solo cuando la clase cierra un ciclo temático completo (p. ej. el final del bloque de políticas de planificación) y amerita una tabla comparativa o síntesis. Una clase que introduce un tema nuevo sin cerrarlo aún no necesita esta sección.
- **Referencia a OSTEP**: cuando el tema de la clase corresponde directamente a un capítulo del libro *Operating Systems: Three Easy Pieces*, puede mencionarse el capítulo entre paréntesis la primera vez que se nombra el tema (p. ej. "Ejecución Directa Limitada (OSTEP, cap. 6)"). No es obligatorio ni se fuerza si el manuscrito no lo menciona explícitamente.

---

## 3. Reglas de fidelidad a la fuente

- Toda cifra, fórmula, nombre de algoritmo, comando o resultado de ejemplo debe verificarse contra el **manuscrito anotado** como fuente primaria. El resumen de Zoom es secundario/complementario — útil para contexto conversacional (preguntas de estudiantes, aclaraciones dichas en voz alta que no quedaron escritas, pendientes) pero no para datos duros si hay conflicto con el manuscrito.
- Si el manuscrito y el material de Zoom se contradicen en algún dato relevante para el estudiante (una fórmula, un resultado numérico de un ejemplo, el nombre de un algoritmo), señalar la discrepancia explícitamente al profesor antes de resolverla unilateralmente — nunca elegir silenciosamente una versión.
- Discrepancias que son evidentemente errores de transcripción automática de Zoom sin relevancia pedagógica (un símbolo mal transcrito cuando el manuscrito lo muestra con claridad) se resuelven usando el manuscrito como fuente de verdad, sin necesidad de nota visible.
- **El manuscrito también puede tener errores, y no se propagan**: si contiene un error evidente (un cálculo que no cuadra, p. ej. "7K = 7098"; una etiqueta equivocada, p. ej. "heap" sobre los bits de stack; un typo técnico, p. ej. "Worse-fit"; una operación mal escrita, p. ej. "100 + 1K = 100 + 4096"), el apunte muestra el valor **correcto**. La prioridad es que lo que estudian los estudiantes no tenga errores. La corrección no es silenciosa hacia el profesor: cada error encontrado se le lista en la Fase 2 junto con la corrección propuesta. En el documento final no hace falta nota visible, salvo que la corrección cambie un resultado que los estudiantes vieron en clase y pueda confundirlos.
- **Excepción: comandos y salidas de programas efectivamente ejecutados** (simulador, código de ejemplo) se reproducen **tal cual** se mostraron, porque cambiarlos sería inventar una salida. Si difieren del cálculo a mano, se añade una nota que explique la diferencia (p. ej. en `clase_09` el simulador se invocó con `7096` en lugar de 7K = 7168, y traduce la VA 4200 a 36 968 porque junta code y heap en un solo segmento, mientras el ejemplo de clase da 34 920).
- **Verificación independiente**: antes de escribir cualquier cifra, fórmula o conversión (decimal ↔ hexadecimal ↔ binario, potencias de 2, rangos de direcciones, sumas base + offset), recalcularla de forma independiente, sin copiarla de ninguna fuente. Si ninguna fuente coincide con el recálculo, se señala al profesor. Las afirmaciones que hace el propio borrador (p. ej. "los 12 bits bajos de la PA contienen el offset") también se verifican: el borrador no debe introducir errores nuevos.
- **Errores conceptuales del Zoom y del código de las diapositivas**: además de los datos duros, se corrigen las **explicaciones** equivocadas de los resúmenes y notas de Zoom (p. ej. en `clase_17` el Zoom atribuía al `if` la falla de las reimplementaciones del *join*, cuando la causa era la falta de variable de estado o de lock; o afirmaba que el hilo despertado "encontrará el mutex ya liberado", lo cual no está garantizado en una máquina con varios núcleos) y los errores del código de las diapositivas (una llamada sin un argumento, etiquetas de línea corridas, conversiones de puntero a `int` incorrectas en 64 bits). Se contrastan con el capítulo de OSTEP y con el código que compila en `ejemplos/`, y se listan al profesor en la Fase 2.
- **Origen de cada salida de programa**: toda salida citada en el apunte declara de dónde viene: (a) **ejecutada en la sesión** (captura pegada en el manuscrito o salida citada en el Zoom); (b) **captura incluida en el deck** (viene en las diapositivas originales, aunque en la sesión se haya ejecutado el mismo escenario); (c) **ejecución de referencia**, con fecha y entorno (la de las guías de `ejemplos/`, Sección 7). Las salidas se copian tal cual de la fuente, sin editarlas; si se recorta, se marca con `...`. Si el Zoom dice que algo se ejecutó pero no da la salida, se menciona sin cifras inventadas. En programas concurrentes se presentan como "posibles salidas" y se explica qué parte varía entre ejecuciones.
- **Banderas y comportamiento de los simuladores**: se verifican contra el código fuente del simulador en el repositorio (`add_option`, lógica de traducción), no contra lo que dicen los resúmenes de Zoom (p. ej. en `clase_09` el resumen atribuía a `-c` el paso de direcciones, que en realidad es `-A`).
- El orden de las secciones temáticas sigue el orden real de dictado (manuscrito), que puede no coincidir con el orden de la diapositiva de agenda original — la diapositiva refleja el plan, el manuscrito refleja lo que realmente ocurrió.
- No inventar ni completar ejemplos, resultados de simulador o pasos de un cálculo que no fueron mostrados en clase, aunque sea "lo esperable" para el tema.

---

## 4. Flujo de trabajo (fases)

**Fase 0 — Revisión de la carpeta `ejemplos/`** (clases con código; a pedido del profesor)
Normalmente se hace cuando la clase empieza a dictarse, antes del apunte, y puede ser una tarea independiente. Claude revisa, corrige, verifica en WSL y documenta la carpeta `clase_NN/ejemplos/` siguiendo la Sección 7. Si al llegar a la Fase 1 la carpeta todavía no se ha revisado (sigue llamándose `code/`, no tiene guías o sus salidas no son de una ejecución verificada), Claude lo advierte y propone hacer la Fase 0 primero, porque el apunte va a enlazar esas guías y citar sus salidas.

**Fase 1 — Recepción de material**
Claude crea la carpeta `clase_NN/apuntes_zoom/` y confirma qué insumos ya están en el repositorio (manuscrito anotado, diapositivas, `apuntes/`, simulador, `ejemplos/` con su tabla de páginas → programas) y cuáles necesita del profesor. Para armar la checklist, identifica las sesiones a partir de las marcas del manuscrito ("Inicio", "Fin", "Next"), de qué páginas tienen anotaciones (contando los trazos `<stroke>` por página del `.xopp`) y, si una sesión siguió con la clase siguiente, de las marcas del manuscrito de esa clase. El profesor entrega el material de Zoom **de forma iterativa, un insumo por mensaje** (p. ej. resumen automático del día 1, notas del día 1, resumen del día 2, notas del día 2). Por cada insumo Claude solo confirma la recepción con una línea que describa su contenido, **sin sacar conclusiones ni cruzar fuentes todavía**, y espera el siguiente. Excepción: si un insumo llega con una fecha que contradice lo que se sabe (p. ej. un día en que no hubo clase, o un texto idéntico a uno ya recibido), Claude lo señala en esa misma confirmación, porque determina a qué sesión pertenece; precedente: en `clase_17` el resumen del 19/11 llegó rotulado "nov 17", un día sin clase. Cuando el profesor avisa "material de Zoom listo", Claude presenta una checklist de insumos recibidos y pendientes.

**Fase 2 — Extracción y verificación**
Claude lee el manuscrito anotado **completo** y lo cruza con todos los insumos de Zoom, con el capítulo de OSTEP (descargado de `pages.cs.wisc.edu/~remzi/OSTEP/`, para citar secciones y figuras exactas) y con el material del repositorio (incluidos el código y las guías de `ejemplos/`), aplicando las reglas de la Sección 3. Para leer el manuscrito conviene renderizar las páginas a imagen (`pdftoppm -r 72 -png`) y agruparlas de a cuatro, y ampliar (`-r 130`) las zonas con trazas o texto pequeño; el texto de las diapositivas se extrae con `pdftotext -layout`. Antes de redactar, presenta al profesor en un solo mensaje:
- Qué confirma el manuscrito (p. ej. el corte de sesión) y los errores de transcripción de Zoom que se resolverán con el manuscrito sin nota.
- Los errores encontrados en el propio manuscrito y la corrección propuesta.
- Las discrepancias relevantes entre fuentes y las decisiones abiertas (qué incluir de un repaso, de una diapositiva no comentada, de elementos administrativos, dónde termina la clase si hay un "puente"), cada una con una recomendación.

**Fase 3 — Borrador**
Claude genera el `README.md` completo siguiendo la plantilla de la Sección 2, ubicado en la ruta correcta (`clase_NN/apuntes_zoom/README.md` o `clase_NN/apuntes_zoom/<subtema>/README.md`), e incluye las imágenes referenciadas como archivos hermanos en `img/` y los diagramas Mermaid inline en el propio Markdown. Antes de entregarlo, lo **revisa completo**: recalcula cada cifra y conversión, comprueba que cada enlace relativo apunte a un archivo existente, y confirma que ninguna afirmación propia del borrador sea incorrecta. Informa al profesor qué corrigió en esa revisión. Herramientas que se usan en esta revisión:

- **Enlaces y anclas**: un script corto (Python) que recorre los enlaces `](...)` del borrador, comprueba que cada ruta exista y que cada `#ancla` coincida con un encabezado del archivo destino, calculada como lo hace GitHub (minúsculas; se quitan los signos que no son letras, números, espacios, `-` ni `_`; los espacios pasan a `-`). Las anclas hacia las guías de `ejemplos/` son las que más se rompen, porque sus títulos llevan páginas y nombres de archivo.
- **Diagramas Mermaid**: se renderizan uno por uno con `mermaid-cli` (`mmdc`), instalado en el directorio temporal de la sesión (`npm install @mermaid-js/mermaid-cli`), y se revisa la imagen de los diagramas con cuadros. Si las flechas se cruzan y las etiquetas se enciman (pasa en `flowchart` con flechas cruzadas), se reemplaza el diagrama por una tabla.
- **Coherencia con las guías**: si al cruzar fuentes aparece un error o una imprecisión en una guía de `ejemplos/`, se corrige también allí y se informa (precedente: en `clase_17` una guía llamaba "ejecución en vivo" a una captura que venía en el deck).

**Fase 4 — Revisión y ajuste**
El profesor revisa y solicita ajustes de forma (tono, tablas vs. listas, reordenamiento) o de fondo (correcciones de contenido). Los ajustes se aplican en rondas, no uno a uno sin confirmación previa cuando son varios.

**Fase 5 — Cierre**
Una vez aprobado, el archivo queda como definitivo para esa clase y se actualiza la fila correspondiente en el `README.md` raíz (tabla de "Apuntes por clase", dividida por módulos) agregando el enlace a `apuntes_zoom/` junto al ya existente hacia `apuntes/` (cuando ambas carpetas existan para esa clase). Si la clase aún no tiene fila, se crea una por sesión, siguiendo el formato de la tabla; las fechas se toman del manuscrito o de los resúmenes de Zoom, o se deducen del horario (martes y jueves) y se le indican al profesor. Se verifica que **todos** los enlaces de la tabla existan y apunten al contenido que nombra su fila; en las clases con código, la fila incluye también el enlace `[[ejemplos]]`. Los días sin clase no llevan fila, pero se anotan en `CLAUDE.md` (precedentes: 08/10 y 17/11). Por último, se actualiza `CLAUDE.md` (qué clases tienen `apuntes_zoom/` y cualquier convención nueva) y se hace un commit por cada cambio independiente. Clases anteriores ya publicadas no se reformatean retroactivamente solo para ajustarse a una versión más nueva de este prompt maestro salvo que el profesor lo pida explícitamente — cada `README.md` es un registro fiel de lo dictado, generado con las convenciones vigentes en su momento.

---

## 5. Convenciones heredadas del repositorio

- Los apuntes están escritos en **español** (ver `CLAUDE.md` del repositorio).
- Tablas en Markdown estándar (`|---|---|`); LaTeX inline (`$...$`, `$$...$$`) para fórmulas, ya usado en `clase_05/apuntes/`.
- **Estructura de archivos por clase**: `clase_NN/` contiene las diapositivas y anotaciones (`.pptx`, `.pdf`, `.xopp`, `_annotated.pdf`) como hermanos de `apuntes/` y, a partir de esta versión, de `apuntes_zoom/`. Dentro de `apuntes_zoom/` van el/los `README.md` y su carpeta `img/`; el código o simulador de la clase sigue viviendo únicamente bajo `apuntes/` o en su carpeta hermana `simulador/`/`simulacion/`/`ejemplos/` (ver `CLAUDE.md` para el detalle de cada simulador y sus banderas CLI, y la Sección 7 para `ejemplos/`) — no se duplica dentro de `apuntes_zoom/` (única excepción, a pedido del profesor: `clase_14/apuntes_zoom/ejemplos/`).
- Los simuladores Python bajo `simulacion/`/`simulador/`/`lab/` son copias adaptadas de los homeworks de OSTEP — al citarlos en el apunte, preservar la interfaz de línea de comandos documentada en `CLAUDE.md`.

---

## 6. Preguntas abiertas

- ~~¿Las clases ya publicadas sin `apuntes_zoom/` se completan retroactivamente?~~ **Resuelta**: sí, `apuntes_zoom/` se generó retroactivamente para `clase_02` a `clase_08`, y desde `clase_09` se genera con esta versión.
- ¿Los `apuntes_zoom/` generados con versiones anteriores (v1.0/v1.1, `clase_02` a `clase_08`) se revisan con la regla de v1.2 de no propagar errores del manuscrito? (Fase 5 asume que no se reformatean salvo pedido explícito, pero corregir un error de contenido que los estudiantes están estudiando podría justificarlo.)
- ¿Las clases ya publicadas sin badge/objetivos en `apuntes/` (`clase_02`, `clase_03`, `clase_04`) se migran alguna vez a ese formato, o quedan como están por ser registro histórico? (Sección 4, Fase 5, asume que quedan como están salvo pedido explícito — aplica solo a `apuntes/`, no a `apuntes_zoom/`, que es contenido nuevo.)

---

## 7. Carpeta `ejemplos/`: revisión, verificación en WSL y documentación

Desde la clase 14, el código de cada clase vive en `clase_NN/ejemplos/`, y las clases que faltan (la 18 en adelante) seguirán ese patrón. Esta sección recoge el procedimiento que se aplicó a `clase_16/ejemplos/` (11/11/2026) y a `clase_17/ejemplos/` (19/11/2026), para repetirlo en cada clase nueva. Tiene tres objetivos:

1. **Que el código sea correcto**: que compile sin advertencias y haga lo que dicen las diapositivas, corrigiendo los errores que trae del semestre pasado.
2. **Que el estudiante pueda reproducir cada comportamiento**, incluidos los errores que la clase quiere mostrar, con salidas reales verificadas en WSL.
3. **Que se llegue al código desde todo el repositorio**: el apunte de Zoom, el `README.md` raíz, el `README.md` de la clase y las guías se enlazan entre sí.

El trabajo se hace a pedido del profesor (Fase 0) y se registra en `CLAUDE.md`. Si la carpeta tiene una nota interna del docente (`NOVEDADES.md`, `ABOUT.md`), se consulta antes de modificarla y se actualiza al terminar.

### 7.1 Inventario y estructura

- **Renombrar**: el material del semestre pasado suele llegar como `code/`. Se renombra con `git mv code ejemplos`, en un commit que solo contenga el renombre, para que git lo registre como tal y no como borrado más creación.
- **Quitar lo que sobra**: archivos `.zip` con versiones viejas de los programas, ejecutables, objetos (`.o`) y `.ipynb_checkpoints/`. Antes de borrar un `.zip`, se compara su contenido con los archivos sueltos: puede traer una versión distinta que valga la pena conservar.
- **Una carpeta por tema**, con su `Makefile`. Las cabeceras auxiliares (`mythreads.h`, `common.h`, ...) van en cada carpeta si las diapositivas compilan con `gcc -I.` (así el comando de la diapositiva funciona desde cualquier carpeta). Esas copias deben ser idénticas, cosa que se verifica con `cmp` o comparando su hash. Si no hace falta el `-I.`, se usa una sola carpeta `include/` (precedente: `clase_16/ejemplos/counters/`).
- **Relación diapositivas → programas**: para cada página con código se identifica qué archivo la implementa. Si una página no tiene programa, o un programa no está en las diapositivas, se anota; si hace falta crear uno, se consulta al profesor.

### 7.2 Revisión del código

- **Por defecto, el código de las diapositivas.** Sin argumentos, cada programa hace exactamente lo que muestra la diapositiva, para que coincida con lo visto en clase.
- **Corregir los errores** frente a las diapositivas y a OSTEP (p. ej. en `clase_17`, `implementacion3.c` usaba `if` donde las diapositivas y OSTEP usan `while`), los usos incorrectos de la API y las conversiones que fallan en 64 bits. Cada corrección se explica en la guía con un recuadro "Corrección frente al material del semestre pasado".
- **Argumentos opcionales que fuerzan los escenarios.** Si la clase muestra un error que depende del entrelazado de los hilos, el programa acepta un argumento opcional (un retardo en segundos o milisegundos, insertado justo en la ventana de la carrera) que lo hace reproducible. La pausa solo se ejecuta si el argumento es mayor que cero. En `clase_17`, una llamada a `sleep(0)` (que no espera, pero es una llamada al sistema que le da al planificador la ocasión de cambiar de hilo) hizo que una versión se bloqueara en 261 de 500 ejecuciones.
- **Variantes por compilación condicional.** Las variantes de un mismo programa (con traza, sin `signal`, con `signal` doble, ...) se generan con `-D` desde objetivos del `Makefile` (`make traza`), en lugar de copiar el archivo.
- **`Makefile` por carpeta.** Usa `CC = gcc`, `CFLAGS` con `-Wall` y `-pthread` (y `-I.` si aplica), ejecutables con extensión `.out` (el `.gitignore` excluye `*.out`), los objetivos `all` y `clean` declarados en `.PHONY`, y dependencias de las cabeceras. Plantilla usada en `clase_17`:

  ```make
  CC          = gcc
  CFLAGS      = -Wall -I. -pthread
  SOURCES     = $(wildcard *.c)
  EXECUTABLES = $(SOURCES:.c=.out)

  .PHONY: all clean

  all: $(EXECUTABLES)

  %.out: %.c mythreads.h
  	$(CC) $(CFLAGS) -o $@ $<

  clean:
  	rm -f $(EXECUTABLES)
  ```

  Si las diapositivas compilan con otros nombres de ejecutable (`-o bounded-buffer.out`), la guía índice incluye una tabla con la equivalencia entre el comando de cada diapositiva y el ejecutable del `Makefile`.
- **Equivalencias que se mantienen**: las versiones C y Python (`clase_14`, `clase_15`), las copias de cabeceras y los programas que deben ser idénticos (p. ej. `sin_locks/main.c` y `con_locks/main.c` en `clase_16`) se modifican juntos.

### 7.3 Verificación en WSL

- **Máquina de referencia**: WSL1 sobre Windows 10 (Ubuntu 20.04, gcc 9.4, 8 CPU lógicas), descrita en [`clase_14/ejemplos/entorno_ejecucion.md`](clase_14/ejemplos/entorno_ejecucion.md). Las guías **enlazan** ese archivo en vez de repetir la descripción. En WSL1, `taskset` y `sched_setaffinity` se aceptan pero no tienen efecto.
- **Invocación desde la terminal de Claude** (Git Bash en Windows), con el repositorio montado en `/mnt/d/`:

  ```bash
  MSYS_NO_PATHCONV=1 wsl -e bash -lc "cd /mnt/d/UdeA/2026-2/repos/apuntes_2026-2_SO-virtual/clase_NN/ejemplos/<carpeta> && make clean && make && ./programa.out <args>"
  ```

  `MSYS_NO_PATHCONV=1` evita que Git Bash reescriba las rutas que empiezan con `/`. Dentro de las comillas dobles, los `$` del script de Linux se escriben `\$` para que no los expanda Git Bash. Desde PowerShell basta `wsl -e bash -lc "..."`.
- **Ejecutar todos los programas**, con y sin sus argumentos opcionales, y copiar las salidas **textualmente**, con la fecha de la ejecución. Una salida que no se obtuvo ejecutando el programa no se escribe en una guía.
- **Comportamientos no deterministas**: se repite la ejecución (20, 50, 100 o 500 veces, según el costo) y se reportan los conteos en una tabla "Repeticiones / Terminó / Quedó bloqueado / Falló". Por ejemplo:

  ```bash
  n=0; for i in $(seq 500); do timeout 2 ./programa.out > /dev/null; [ $? -eq 124 ] && n=$((n+1)); done; echo "bloqueos: $n de 500"
  ```

  Un error que aparece pocas veces (2 de 500) se documenta igual: muestra que pasar las pruebas no demuestra que un programa concurrente sea correcto.
- **Programas que se bloquean o no terminan**: `timeout N` (código de salida 124 si tuvo que terminar el programa), `stdbuf -oL` antes de un *pipe* (sin él, al matar el proceso se pierde la salida que estaba en el búfer de `printf`) y conteos con `wc -l` o `sort -u | wc -l`, para distinguir un bloqueo a mitad de camino de un programa que solo se quedó esperando al final.
- **Tiempos**: con `time`, enviando la salida a `/dev/null`. Compilar y ejecutar dentro de `/mnt/d/` es más lento que dentro de `~`, así que para mediciones finas conviene copiar la carpeta a `~` (ver `entorno_ejecucion.md`).
- **Si una salida contradice la diapositiva o la teoría**, se investiga (por ejemplo, con la versión con traza) y se documenta el hallazgo, en vez de forzar el resultado esperado. Precedente: en `clase_17`, la versión con una CV e `if` falla casi siempre porque la señal de un consumidor despierta al otro consumidor, no por el escenario de la diapositiva p. 46.
- **Dejar el árbol limpio**: `make clean` en cada carpeta; `git status` no debe mostrar ejecutables.
- **Si después se cambia el código**, se vuelve a ejecutar y se actualizan las cifras en **todos** los documentos que las repiten: guías, `apuntes_zoom/` y `CLAUDE.md`.

### 7.4 Guías (`README.md`)

- **Índice** (`ejemplos/README.md`): introducción que ubica el tema en el módulo; tabla carpeta → contenido → páginas de las diapositivas → sección de OSTEP; "Ruta sugerida" (diagrama Mermaid que ordena las versiones, marcando en rojo las que fallan y en verde las correctas); requisitos; cómo probar programas que pueden bloquearse (si aplica); tabla de nombres de ejecutables (si aplica); referencias.
- **Una guía por carpeta**: tabla "Relación con las diapositivas y el libro" (programa, qué muestra, páginas, sección y figura de OSTEP v1.10); compilación; una sección por programa o versión, con el archivo y las páginas en el título (p. ej. "4. v2: una variable de condición y `while` (p. 50–58)"); fragmento clave; comando; salida de referencia; tabla de repeticiones; interpretación a la luz de la teoría; diagramas Mermaid de secuencia para los entrelazados que fallan, validados con `mermaid-cli`; recuadros "Complemento (OSTEP, cap. N)" para lo que no está en las diapositivas; "Para probar por cuenta propia"; referencias.
- **Forma**: el badge `![Built with AI](...)` en la primera línea; al final, la nota "Sobre esta guía" en un recuadro `> [!IMPORTANT]`, con la fecha y el entorno de la ejecución de referencia (enlace a `entorno_ejecucion.md`) y la aclaración de que, ante una diferencia con lo visto en clase, prevalece lo explicado por el docente; tono formal e impersonal, sin tuteo; en español; las mismas convenciones de Mermaid de la Sección 2.
- **Notas internas del docente** (`NOVEDADES.md`, `ABOUT.md`): registro de cambios frente al semestre pasado, mediciones, puntos que pueden confundir en clase y erratas del deck. Se agregan al `.gitignore` y no se suben al remoto.

### 7.5 Acceso desde el ecosistema

- **Apunte de Zoom** (`apuntes_zoom/README.md`): un bloque "Cómo verlo" por sección, que enlaza el `.c` y la sección de la guía con su ancla, y la sección "Para probar por cuenta propia" (Sección 2).
- **`README.md` raíz**: el enlace `[[ejemplos]]` en las filas de las sesiones de la clase.
- **`README.md` de la clase** (cuando existe; patrón de `clase_14` y `clase_16`): una "Ruta de lectura" por pasos con las páginas de las diapositivas, las secciones de OSTEP y los programas.
- **Entre guías**: el índice enlaza cada carpeta y cada guía enlaza de vuelta al índice ("Volver al índice de ejemplos"); las guías relacionadas se enlazan entre sí con anclas.
- **`CLAUDE.md`**: una entrada en "Código de ejemplo y simuladores por clase" con la estructura, los comandos, la fecha de la ejecución de referencia, los hallazgos y las reglas de mantenimiento (qué archivos deben seguir idénticos y qué código no se modifica).
- **Commits**: uno con el renombre `code/` → `ejemplos/`, otro con el código y las guías, y otro con `CLAUDE.md` (o este último junto con el anterior, si el cambio es pequeño).

### 7.6 Lista de verificación

- [ ] Carpeta renombrada a `ejemplos/`, sin `.zip`, ejecutables ni *checkpoints*.
- [ ] Cada página de las diapositivas con código tiene su programa, y cada programa tiene su página o una nota que explica por qué no la tiene.
- [ ] Todo compila sin advertencias en WSL con `make`; las copias de cabeceras son idénticas.
- [ ] Todos los programas se ejecutaron; las salidas de las guías se copiaron tal cual y tienen fecha y entorno.
- [ ] Los comportamientos no deterministas tienen su tabla de repeticiones.
- [ ] Las correcciones frente al semestre pasado están explicadas en las guías y en la nota interna del docente.
- [ ] Los enlaces y las anclas de todas las guías resuelven; los diagramas Mermaid se renderizan.
- [ ] `make clean` en todas las carpetas; `git status` no muestra ejecutables.
- [ ] `CLAUDE.md` actualizado; enlaces `[[ejemplos]]` en el `README.md` raíz cuando las sesiones tengan fila.
