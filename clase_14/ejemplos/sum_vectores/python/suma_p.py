# Equivalente en Python de suma_p.c: suma paralela de dos arreglos con hilos.

import os
import sys
import threading
from array import array
from common import GetTime, Spin  # Spin solo se usa si se descomenta abajo

# ======================================================
# Estructura y función del hilo
# ======================================================
class ThreadArgs:
    # Equivalente a thread_args_t en C
    def __init__(self, id, vector1, vector2, vectorSuma, tamano_total, num_hilos):
        self.id = id
        self.vector1 = vector1
        self.vector2 = vector2
        self.vectorSuma = vectorSuma
        self.tamano_total = tamano_total
        self.num_hilos = num_hilos

def sumar_porcion_vector(data):
    tamano_trozo = data.tamano_total // data.num_hilos
    inicio = data.id * tamano_trozo
    fin = inicio + tamano_trozo

    if data.id == data.num_hilos - 1:
        fin = data.tamano_total

    # Referencias locales a los arreglos, como hace suma_s.py; así el ciclo
    # no busca el atributo en data en cada iteración.
    vector1, vector2, vectorSuma = data.vector1, data.vector2, data.vectorSuma
    for i in range(inicio, fin):
        vectorSuma[i] = vector1[i] + vector2[i]
        # Descomenta la siguiente línea para simular una carga de trabajo más pesada
        # Spin(0.000001)

# ======================================================
# Función principal
# ======================================================
def main():
    # Debe ser el mismo valor que en suma_s.py para que Ts y Tp sean comparables.
    TAMANO = 100000000  # 100M → ~1.2 GB, igual que en C.

    try:
        # Inicializar vectores (ver suma_s.py). resultado se crea lleno de
        # ceros para que sus fallos de página ocurran aquí y no en la medición.
        v1 = array('i', range(TAMANO))
        v2 = array('i', range(0, 2 * TAMANO, 2))
        resultado = array('i', [0]) * TAMANO
    except MemoryError:
        print("Error al asignar memoria.")
        return 1

    # Por defecto, un hilo por núcleo lógico; opcionalmente: python3 suma_p.py <num_hilos>
    NUM_HILOS = int(sys.argv[1]) if len(sys.argv) > 1 else os.sysconf("SC_NPROCESSORS_ONLN")
    if NUM_HILOS < 1:
        print(f"uso: {sys.argv[0]} [num_hilos >= 1]", file=sys.stderr)
        return 1
    print(f"Usando {NUM_HILOS} hilos para la suma.")

    # INICIA LA MEDICIÓN DE TIEMPO
    t_inicio = GetTime()

    # Crear los hilos
    hilos = []
    for i in range(NUM_HILOS):
        args_hilo = ThreadArgs(i, v1, v2, resultado, TAMANO, NUM_HILOS)
        hilo = threading.Thread(target=sumar_porcion_vector, args=(args_hilo,))
        hilo.start()
        hilos.append(hilo)

    # Esperar a que todos los hilos terminen
    for hilo in hilos:
        hilo.join()

    # FINALIZA LA MEDICIÓN DE TIEMPO
    t_fin = GetTime()
    tiempo_total = t_fin - t_inicio

    print("Suma paralela completada.")
    print(f"Tiempo de ejecución paralela (Tp): {tiempo_total:f} segundos.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
