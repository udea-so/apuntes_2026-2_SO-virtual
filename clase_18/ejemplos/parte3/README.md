![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# Ejemplos — Clase 18, parte 3: lectores-escritores, filósofos y Zemáforos

Tres usos más de los semáforos, siguiendo las diapositivas de la parte 2 (p. 26–48) y OSTEP §31.5, §31.6 y §31.8.

Volver al [índice de ejemplos de la clase 18](../README.md).

| Carpeta | Contenido | Diapositivas | OSTEP cap. 31 |
|---|---|---|---|
| [`reader_writer_locks/`](reader_writer_locks/README.md) | Lock de lectores-escritores (`rwlock_t`): muchos lectores a la vez o un solo escritor. Incluye una variante que muestra la inanición de los escritores. | p. 26–34 | §31.5, Fig. 31.13 |
| [`dining_philosophers/`](dining_philosophers/README.md) | Filósofos comensales: la solución que hace *deadlock* y la que rompe la espera circular cambiando el orden de un filósofo. | p. 35–46 | §31.6, Figs. 31.14–31.16 |
| [`zemaforos/`](zemaforos/README.md) | Un semáforo construido con un lock y una variable de condición, probado como lock, para ordenar hilos y con varios hilos esperando. No estaba en el material del semestre pasado. | p. 47–48 | §31.8, Fig. 31.17 |

## Referencias

- Diapositivas de la clase 18, parte 2, p. 26–48.
- R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, v1.10, cap. 31: [*Semaphores*](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf).

> [!IMPORTANT]
> **Sobre esta guía.** La guía se elaboró con asistencia de un modelo de inteligencia artificial y se revisó contra las diapositivas de la clase y OSTEP. Si se encuentra alguna diferencia con lo visto en clase, prevalece lo explicado por el docente.
