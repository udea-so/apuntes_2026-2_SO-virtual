![Built with AI](https://img.shields.io/badge/Built%20with-AI-blue.svg)

# El código original de OSTEP

Esta carpeta conserva **sin modificar** [`producer_consumer_works.c`](producer_consumer_works.c), la solución del productor-consumidor de [`ostep-code/threads-sema`](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-sema). Sirve como referencia para comparar con [`pc_work`](../circular_buffer/pc_work/README.md), que es la misma solución adaptada a las diapositivas.

Volver al [índice de la parte 2](../../README.md).

## Compilación y uso

```bash
make          # genera producer_consumer_works.out
make clean

./producer_consumer_works.out <buffersize> <loops> <consumers>
```

Hay un solo productor. Cada consumidor imprime su número (empezando en 0) y el valor que sacó:

```text
$ ./producer_consumer_works.out 10 5 2
0 0
0 2
0 3
0 4
0 -1
1 1
1 -1
```

## Diferencias con `pc_work`

| | `producer_consumer_works.c` (OSTEP) | `pc_work.c` |
|---|---|---|
| Sincronización | `empty`, `full` y `mutex`, igual que la Fig. 31.12 | La misma |
| Rutinas del buffer | `do_fill()` y `do_get()` (con `if` en lugar de `%`) | `put()` y `get()` de las diapositivas |
| Productores | Uno | Los que se indiquen |
| Marca de fin | La pone el productor (un `-1` por consumidor) | La pone `main` cuando terminaron todos los productores |
| Comprobación | Ninguna | [`verifica.h`](../single_buffer/verifica.h) |

La marca de fin en el productor solo es correcta con un productor: con dos, los consumidores pueden recibir su `-1` antes de que el otro productor termine (ver el [índice de la parte 2](../../README.md#uso-común-de-los-programas)).

## Referencias

- R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, v1.10, cap. 31: [*Semaphores*](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf), Fig. 31.12.

> [!IMPORTANT]
> **Sobre esta guía.** La guía se elaboró con asistencia de un modelo de inteligencia artificial. La salida es de una ejecución de referencia del 23/11/2026 en la máquina descrita en el [entorno de ejecución](../../../../../clase_14/ejemplos/entorno_ejecucion.md) de la clase 14.
