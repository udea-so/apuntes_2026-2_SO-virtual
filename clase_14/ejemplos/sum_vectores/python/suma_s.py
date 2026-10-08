# Equivalente en Python de suma_s.c: suma serial de dos arreglos.

from array import array
from common import GetTime, Spin  # Spin solo se usa si se descomenta abajo

def main():
    # Debe ser el mismo valor que en suma_p.py para que Ts y Tp sean comparables.
    TAMANO = 100000000  # 100M → ~1.2 GB, igual que en C.

    # array('i') guarda enteros de 4 bytes contiguos en memoria, como
    # malloc(TAMANO * sizeof(int)) en C. Una lista de Python guardaría un
    # objeto por elemento y ocuparía varias veces más memoria.
    try:
        # Inicializar vectores. resultado se crea lleno de ceros: equivale a
        # resultado[i] = 0 en C (sus fallos de página ocurren aquí y no
        # dentro de la medición).
        v1 = array('i', range(TAMANO))                # v1[i] = i
        v2 = array('i', range(0, 2 * TAMANO, 2))      # v2[i] = i * 2
        resultado = array('i', [0]) * TAMANO
    except MemoryError:
        print("Error al asignar memoria.")
        return 1

    # ======================================================
    # Medición de la versión serial
    # ======================================================

    print("Ejecutando la suma en modo serial...")

    # INICIA LA MEDICIÓN
    t_inicio_serial = GetTime()

    # Un único bucle que hace todo el trabajo
    for i in range(TAMANO):
        resultado[i] = v1[i] + v2[i]
        # Descomenta la siguiente línea para simular una carga de trabajo más pesada
        # (debe hacerse igual en suma_p.py para comparar)
        # Spin(0.000001)

    # FINALIZA LA MEDICIÓN
    t_fin_serial = GetTime()
    tiempo_serial = t_fin_serial - t_inicio_serial

    print(f"Tiempo de ejecución serial (Ts): {tiempo_serial:f} segundos.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
