#ifndef __verifica_h__
#define __verifica_h__

/*
 * verifica.h - Registro y verificacion de los valores consumidos.
 *
 * No es parte de la sincronizacion: solo sirve para comprobar el resultado.
 * Cada productor pone los valores 0, 1, ..., loops-1, asi que al final cada
 * valor debe haberse consumido exactamente <productores> veces. Cada
 * consumidor cuenta en su propio arreglo (no se comparte con nadie) y main
 * suma los arreglos cuando todos los hilos terminaron.
 *
 * Las copias de este archivo en las carpetas del productor-consumidor son
 * identicas y deben mantenerse asi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PMAX 10   // maximo de productores
#define CMAX 10   // maximo de consumidores

typedef struct {
    int id;          // 1, 2, ... (C1, C2, ...)
    int loops;
    int *conteo;     // conteo[v]: veces que este consumidor saco el valor v
    int invalidos;   // valores fuera de 0..loops-1 que no son la marca -1
} consumidor_t;

void consumidor_init(consumidor_t *c, int id, int loops) {
    c->id = id;
    c->loops = loops;
    c->conteo = calloc(loops > 0 ? loops : 1, sizeof(int));
    assert(c->conteo != NULL);
    c->invalidos = 0;
}

void registra(consumidor_t *c, int v) {
    if (v == -1)
        return;                       // marca de fin
    if (v >= 0 && v < c->loops)
        c->conteo[v]++;
    else
        c->invalidos++;
}

void verifica(consumidor_t c[], int consumers, int producers, int loops) {
    long esperados = (long) producers * loops;
    long consumidos = 0, perdidos = 0, repetidos = 0, invalidos = 0;
    int v, k;
    for (v = 0; v < loops; v++) {
        int total = 0;
        for (k = 0; k < consumers; k++)
            total += c[k].conteo[v];
        consumidos += total;
        if (total < producers)
            perdidos += producers - total;
        else
            repetidos += total - producers;
    }
    for (k = 0; k < consumers; k++) {
        invalidos += c[k].invalidos;
        free(c[k].conteo);
    }
    consumidos += invalidos;
    printf("Verificacion: producidos %ld, consumidos %ld, perdidos %ld, "
           "repetidos %ld, invalidos %ld -> %s\n",
           esperados, consumidos, perdidos, repetidos, invalidos,
           (perdidos == 0 && repetidos == 0 && invalidos == 0) ? "CORRECTO" : "INCORRECTO");
}

#endif // __verifica_h__
