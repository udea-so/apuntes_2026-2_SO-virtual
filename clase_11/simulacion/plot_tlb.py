#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
plot_tlb.py — Grafica los resultados de tlb_bench.c (metodo de Saavedra-Barrera)
en un estilo comparable a la Figura 19.5 de OSTEP (capitulo 19, "Paging:
Faster Translations (TLBs)").

Uso:
    python3 plot_tlb.py resultados.csv resultados.png

Lee un CSV con columnas: numpaginas,bytes,ns_por_acceso
y produce un PNG con:
  - eje X: numero de paginas tocadas (escala log2)
  - eje Y: tiempo promedio por acceso (ns)
  - etiquetas directas en los "escalones" detectados (posibles limites de
    TLB L1 / L2)

No requiere pandas: solo el modulo estandar csv + matplotlib.
"""

import sys
import csv
import math
import argparse

import matplotlib
matplotlib.use("Agg")  # no requiere entorno grafico
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker


# Paleta validada (dataviz skill) - serie unica, un solo hue.
COLOR_SURFACE = "#fcfcfb"
COLOR_INK_PRIMARY = "#0b0b0b"
COLOR_INK_SECONDARY = "#52514e"
COLOR_INK_MUTED = "#898781"
COLOR_GRIDLINE = "#e1e0d9"
COLOR_BASELINE = "#c3c2b7"
COLOR_SERIES = "#2a78d6"  # blue, slot 1


def leer_csv(ruta):
    paginas, bytes_, ns = [], [], []
    with open(ruta, newline="", encoding="utf-8") as f:
        lector = csv.DictReader(f)
        for fila in lector:
            paginas.append(int(fila["numpaginas"]))
            bytes_.append(int(fila["bytes"]))
            ns.append(float(fila["ns_por_acceso"]))
    return paginas, bytes_, ns


def detectar_escalones(paginas, ns, umbral_razon=1.4):
    """
    Detecta puntos donde el tiempo por acceso sube de forma notable
    respecto al punto anterior (posibles limites de un nivel de TLB).
    Devuelve una lista de (indice, paginas_antes, razon).
    """
    escalones = []
    for i in range(1, len(ns)):
        if ns[i - 1] <= 0:
            continue
        razon = ns[i] / ns[i - 1]
        if razon >= umbral_razon:
            escalones.append((i, paginas[i - 1], razon))
    return escalones


def formatear_paginas(valor, _pos=None):
    if valor >= 1024 and (valor % 1024 == 0):
        return f"{int(valor // 1024)}K"
    return f"{int(valor)}"


def graficar(paginas, ns, ruta_salida, titulo_extra=""):
    fig, ax = plt.subplots(figsize=(9, 5.5), dpi=150)
    fig.patch.set_facecolor(COLOR_SURFACE)
    ax.set_facecolor(COLOR_SURFACE)

    # Linea principal (serie unica -> no hace falta leyenda)
    ax.plot(
        paginas, ns,
        color=COLOR_SERIES,
        linewidth=2,
        marker="o",
        markersize=6,
        markerfacecolor=COLOR_SERIES,
        markeredgecolor=COLOR_SURFACE,
        markeredgewidth=1,
        solid_capstyle="round",
        zorder=3,
    )

    ax.set_xscale("log", base=2)
    ax.xaxis.set_major_formatter(mticker.FuncFormatter(formatear_paginas))

    # Grid recesivo
    ax.grid(True, which="major", axis="both", color=COLOR_GRIDLINE, linewidth=1, zorder=0)
    ax.set_axisbelow(True)

    for spine_name in ("top", "right"):
        ax.spines[spine_name].set_visible(False)
    for spine_name in ("left", "bottom"):
        ax.spines[spine_name].set_color(COLOR_BASELINE)

    ax.tick_params(colors=COLOR_INK_MUTED, labelsize=9)
    ax.set_xlabel("Número de páginas tocadas (escala log2)", color=COLOR_INK_SECONDARY, fontsize=10)
    ax.set_ylabel("Tiempo por acceso (ns)", color=COLOR_INK_SECONDARY, fontsize=10)

    titulo = "Medición empírica de la TLB — método de Saavedra-Barrera"
    if titulo_extra:
        titulo += f"\n{titulo_extra}"
    ax.set_title(titulo, color=COLOR_INK_PRIMARY, fontsize=12, fontweight="bold", loc="left", pad=14)

    # Etiquetas directas en los escalones detectados
    escalones = detectar_escalones(paginas, ns)
    for (i, paginas_antes, razon) in escalones:
        x, y = paginas[i], ns[i]
        ax.annotate(
            f"salto x{razon:.1f}\n(> {paginas_antes} páginas)",
            xy=(x, y),
            xytext=(12, 18),
            textcoords="offset points",
            fontsize=8.5,
            color=COLOR_INK_SECONDARY,
            arrowprops=dict(arrowstyle="-", color=COLOR_INK_MUTED, linewidth=0.8),
        )

    fig.tight_layout()
    fig.savefig(ruta_salida, facecolor=COLOR_SURFACE)
    return escalones


def main():
    ap = argparse.ArgumentParser(description="Grafica resultados de tlb_bench.c")
    ap.add_argument("csv_entrada", help="CSV generado por tlb_bench (numpaginas,bytes,ns_por_acceso)")
    ap.add_argument("png_salida", nargs="?", default="resultados.png", help="Archivo PNG de salida")
    args = ap.parse_args()

    paginas, bytes_, ns = leer_csv(args.csv_entrada)
    if len(paginas) < 2:
        print("Error: el CSV tiene muy pocos puntos para graficar.", file=sys.stderr)
        sys.exit(1)

    escalones = graficar(paginas, ns, args.png_salida)

    print(f"Grafica guardada en: {args.png_salida}")
    if escalones:
        print("\nEscalones detectados (posibles limites de nivel de TLB):")
        for (i, paginas_antes, razon) in escalones:
            print(f"  - entre {paginas_antes} y {paginas[i]} paginas: "
                  f"el tiempo por acceso sube x{razon:.2f} "
                  f"({ns[i-1]:.2f} ns -> {ns[i]:.2f} ns)")
    else:
        print("\nNo se detectaron escalones claros con el umbral actual "
              "(prueba a bajar 'umbral_razon' en detectar_escalones si esperabas ver alguno).")


if __name__ == "__main__":
    main()
