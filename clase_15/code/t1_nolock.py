import threading
import sys
import time

# python3 t1_nolock.py 1000


# Necesario usar un objeto para que sea mutable y compartido entre hilos
class Counter:
    def __init__(self):
        self.value = 0

counter = Counter()
max_count = 0  # renombrado para no pisar el built-in max() de Python

def mythread(arg):
    print(f"{arg}: begin")
    for i in range(max_count):
        # SIN lock: multiples hilos pueden leer, incrementar y
        # escribir counter.value al mismo tiempo.
        #
        # Equivalente en Python a las 3 instrucciones de ensamblador:
        #   mov 0x<addr>, %eax     <- (1) lee counter desde memoria
        #   add $0x1,     %eax     <- (2) incrementa el registro
        #   mov %eax,     0x<addr> <- (3) escribe el resultado en memoria
        #
        # time.sleep(0) libera el GIL entre (1) y (3), permitiendo que
        # el otro hilo ejecute sus 3 pasos completos y pise el valor.
        # Es el equivalente Python de sched_yield() en C.
        #
        # Nota: el GIL de CPython reduce (pero no elimina) las race
        # conditions. Sin time.sleep(0) pueden no observarse con 2 hilos.
        tmp = counter.value   # (1) lee
        time.sleep(0)         # libera el GIL — el otro hilo corre aqui
        tmp = tmp + 1         # (2) incrementa la copia local
        counter.value = tmp   # (3) escribe -- puede pisar el valor del otro hilo
    print(f"{arg}: done")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: python3 t1_nolock.py <loopcount>")
        sys.exit(1)

    max_count = int(sys.argv[1])
    print("main: begin")
    print(f"main: begin [counter = {counter.value}]")

    p1 = threading.Thread(target=mythread, args=('A',))
    p2 = threading.Thread(target=mythread, args=('B',))
    p1.start()
    p2.start()
    p1.join()
    p2.join()

    print(f"main: done")
    print(f"[counter: {counter.value}]")
    print(f"[should:  {max_count * 2}]")