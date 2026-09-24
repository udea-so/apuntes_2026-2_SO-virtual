# Apuntes clases - Sistemas Operativos (Ude@)

|   |Horario|
|---|---|
|Teoria|MJ 6-8|
|Laboratorio|L 16-18|

## Apuntes por clase

### Modulo 1 - Virtualización de CPU

En la siguiente tabla se encuentran los apuntes a mano de cada sesión del modulo 1:

|Semana	| Clase	| Fecha | Tema | Notas de clase | Observaciones |
|----|----|----|----|----|----|
|1	 | 1 | 04/08/2026 | Clase 1 - Presentación del curso | --- | --- |
|	 | 2 | 06/08/2026 | Clase 2 - Introducción a los sistemas operativos | [[pdf]](clase_02/SO_apuntes_clase2_annotated.pdf) [[apuntes]](clase_02/apuntes/) [[apuntes_zoom]](clase_02/apuntes_zoom/) | Definición y caracteristicas de un sistema operativo|
|2	 | 3 | 11/08/2026 | Clase 3 - Procesos | Apuntes clase 3 [[pdf]](clase_03/SO_apuntes_clase3_annotated.pdf) [[apuntes]](clase_03/apuntes/) [[apuntes_zoom]](clase_03/apuntes_zoom/) [[simulacion]](clase_03/simulacion/) | Diferencia entre programas y procesos, Abstracción de un proceso, estructuras de datos relacionadas (lista de procesos, PCB, etc.)  |
|    | 4 | 13/08/2026 | Clase 4 - Ejecución Directa Limitada (LDE) - Parte 1 | Apuntes clase 4 [[pdf]](clase_04/SO_apuntes_clase4_annotated.pdf) [[apuntes]](clase_04/apuntes/) [[apuntes_zoom]](clase_04/apuntes_zoom/) | Se explico el concepto de ejecucion directa, luego se continuo con el nuevo concepto de ejecucion directa limitada y como se implementa (modos kernel/usuario, bit de modo), finalmente se hablo del cambio entre modos mediantes llamadas de sistema |
|3   | 5 | 18/08/2026 | Clase 5 - Ejecución Directa Limitada (LDE) - Parte 2| Apuntes clase 4 [[pdf]](clase_04/SO_apuntes_clase4_annotated.pdf) [[apuntes]](clase_04/apuntes/) [[apuntes_zoom]](clase_04/apuntes_zoom/) | Se continuo con el cambio entre modos mediante interrupciones, se hablo de los modos cooperativo y no cooperativo, se hablo del concepto de timer y se condenso todo en el protocolo de ejecución directa limitada completo |
|    | 6 | 20/08/2026 | Clase 6 - Politicas de planificación | Apuntes clase 5 [[pdf]](clase_05/SO_apuntes_clase5_annotated.pdf) [[apuntes]](clase_05/apuntes/) [[apuntes_zoom]](clase_05/apuntes_zoom/) | Se definieron los puntos de partida ideales y ha medida que se avanzaba en la clase estos se iban relajando, se vieron las politicas de planificación: FCFS, SJF, STCF y se compararon bajo la metrica Turn around time. |
|4   | 7 | 25/08/2026 | Clase 7 - Multi-Level Feedback Queue (MLFQ) - Parte 1 | Apuntes clase 6 [[pdf]](clase_06/SO_apuntes_clase6_annotated.pdf) [[apuntes]](clase_06/apuntes/mlfq/) [[apuntes_zoom]](clase_06/apuntes_zoom/) | Se vio la metrica de Response Time, se estudio la politica faltante de Round Robin (RR). Asi mismo se relajaron 4 de los 5 puntos de partida ideales. La clase se analizo con procesos interactivos, pues todos los anteriormente analizados eran tipo bash. Al finar se mostro el caso de interleaving.|
|    | 8 | 27/08/2026 | Clase 7 - Multi-Level Feedback Queue (MLFQ) - Parte 2 | Apuntes clase 6 [[pdf]](clase_06/SO_apuntes_clase6_annotated.pdf) [[apuntes]](clase_06/apuntes/mlfq/) [[apuntes_zoom]](clase_06/apuntes_zoom/) | --- |

### Modulo 2 - Virtualización de memoria

En la siguiente tabla se encuentran los apuntes a mano de cada sesión del modulo 2:

|Semana	| Clase	| Fecha | Tema | Notas de clase | Observaciones |
|----|----|----|----|----|----|
|5   | 9 | 01/09/2026 | Clase 7 - Memoria virtual y espacio de direcciones - Parte 1 | Apuntes clase 7 [[pdf]](clase_07/SO_apuntes_clase7_annotated.pdf) [[apuntes]](clase_07/apuntes/address_spaces/) [[apuntes_zoom]](clase_07/apuntes_zoom/) | Se cerro el modulo 1 con un repaso, se planteo el problema de la virtualización de memoria (demostración con mem.c), el concepto general de traducción de direcciones y la evolución de un solo proceso a la multiprogramación. |
|    | 10 | 03/09/2026 | Clase 7 - Memoria virtual y espacio de direcciones - Parte 2 | Apuntes clase 7 [[pdf]](clase_07/SO_apuntes_clase7_annotated.pdf) [[apuntes]](clase_07/apuntes/address_spaces/) [[apuntes_zoom]](clase_07/apuntes_zoom/) | Se estudio el espacio de direcciones (address space) y sus segmentos (code, heap, stack), se mostro que las direcciones que ve un programa son siempre virtuales y se hizo un ejercicio de clasificación de variables por segmento. |
|6   | 11 | 08/09/2026 | Clase 8 - Traducción de direcciones (base & bound) | Apuntes clase 8 [[pdf]](clase_08/SO_apuntes_clase8_annotated.pdf) [[apuntes]](clase_08/apuntes/address_translation_base-bound/) [[apuntes_zoom]](clase_08/apuntes_zoom/) [[simulador]](clase_08/simulador/) | Se vio el soporte de hardware (MMU) y la reubicación dinámica con los registros base y bound, se resolvieron ejercicios de traducción, se uso el simulador relocation.py y se describieron las acciones del SO (freelist, PCB). |
|    | 12 | 10/09/2026 | Clase 9 - Segmentación - Parte 1 | Apuntes clase 9 [[pdf]](clase_09/SO_apuntes_clase9_annotated.pdf) [[apuntes]](clase_09/apuntes/segmentation/) [[apuntes_zoom]](clase_09/apuntes_zoom/) [[simulador]](clase_09/simulador/) | Se partio de las limitaciones de base & bound para introducir la segmentación (3 segmentos, 6 registros), se calcularon los bits de las direcciones virtual y física y se tradujeron direcciones de los segmentos code y heap, incluyendo el fallo de segmentación y el pseudocódigo. |
|7   | 13 | 15/09/2026 | Clase 9 - Segmentación - Parte 2 | Apuntes clase 9 [[pdf]](clase_09/SO_apuntes_clase9_annotated.pdf) [[apuntes]](clase_09/apuntes/segmentation/) [[apuntes_zoom]](clase_09/apuntes_zoom/) [[simulador]](clase_09/simulador/) | Se vio el segmento stack (crecimiento negativo), se verificaron los calculos con el simulador segmentation.py, se estudiaron los segmentos compartidos (bits de protección), la fragmentación externa y la compactación. Al final se introdujo la paginación, que continua en la clase 10. |
|    | 14 | 17/09/2026 | Clase 10 - Paginación - Parte 1 | Apuntes clase 10 [[pdf]](clase_10/SO_apuntes_clase10_annotated.pdf) [[apuntes]](clase_10/apuntes/intro-to-paging/) [[apuntes_zoom]](clase_10/apuntes_zoom/) [[simulador]](clase_10/simulador/) | Se definieron los formatos de las direcciones virtual (VPN + offset) y física (PFN + offset), se tradujeron direcciones con la tabla de páginas (VA 21 → PA 117 y un ejercicio de 16 B / 32 B), se vio que cada proceso tiene su propia tabla de páginas y se calculó su tamaño (4 MB por proceso con direcciones de 32 bits). |
|8   | 15 | 22/09/2026 | Clase 10 - Paginación - Parte 2 | Apuntes clase 10 [[pdf]](clase_10/SO_apuntes_clase10_annotated.pdf) [[apuntes]](clase_10/apuntes/intro-to-paging/) [[apuntes_zoom]](clase_10/apuntes_zoom/) [[simulador]](clase_10/simulador/) | Se estudió la estructura de la PTE y sus bits (validez, referencia, modificación, protección y presencia; PTE de x86), el registro PTBR y el protocolo completo de traducción con paginación. |
|    | 16 | 24/09/2026 | Clase 10 - Paginación - Parte 3 / Clase 11 - TLB - Parte 1 | Apuntes clase 10 [[pdf]](clase_10/SO_apuntes_clase10_annotated.pdf) [[apuntes_zoom]](clase_10/apuntes_zoom/) [[simulador]](clase_10/simulador/) Apuntes clase 11 [[apuntes]](clase_11/apuntes/translation-lookaside-buffers/) [[simulacion]](clase_11/simulacion/) | Se verificó la traducción de la VA 21 con el simulador paging-linear-translate.py, se analizó el costo de los dos accesos a memoria por referencia (ejemplo del arreglo de 1000 enteros) y se introdujo la TLB, que continúa en la siguiente sesión. |

