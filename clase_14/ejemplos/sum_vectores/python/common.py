# Equivalente en Python de common.h (OSTEP): GetTime() y Spin().

import time

def GetTime():
    # Como gettimeofday() en common.h: tiempo actual en segundos (float).
    return time.time()

def Spin(howlong):
    # En common.h, Spin recibe un int: Spin(0.000001) se convierte en Spin(0)
    # y no espera nada; solo llama dos veces a GetTime(). int(howlong)
    # reproduce ese mismo comportamiento.
    t = GetTime()
    while (GetTime() - t) < int(howlong):
        pass  # do nothing in loop
