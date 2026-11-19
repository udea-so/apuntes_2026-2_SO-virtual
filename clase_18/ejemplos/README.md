![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Ejemplos — Clase 18: semáforos

Ejemplos en C del uso de **semáforos** (OSTEP, cap. 31, *Semaphores*), organizados en tres partes que siguen el orden del capítulo.

| Parte | Contenido | OSTEP cap. 31 | Estado |
|---|---|---|---|
| [`parte1/`](parte1/README.md) | Semáforos como locks (semáforo binario) y para ordenar la ejecución entre hilos | §31.1–31.3 | Revisada el 19/11/2026: código verificado y guías con salidas de referencia |
| [`parte2/`](parte2/) | El problema del productor-consumidor con semáforos | §31.4 | Material del semestre pasado, **pendiente de revisión** |
| [`parte3/`](parte3/) | Locks de lectores-escritores y el problema de los filósofos comensales | §31.5–31.6 | Material del semestre pasado, **pendiente de revisión** |

Las partes 2 y 3 se revisarán cuando lleguen sus diapositivas. Mientras tanto, sus programas compilan, pero sus guías todavía no se han actualizado ni verificado.

## Referencias

- R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, v1.10, cap. 31: [*Semaphores*](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf).
- Código del capítulo en [`ostep-code/threads-sema`](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-sema).
