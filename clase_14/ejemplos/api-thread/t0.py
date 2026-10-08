import threading
import sys
import time

def mythread(arg):
  print(arg)

if __name__ == "__main__":
  if len(sys.argv) != 1:
    print("usage: main\n")
  else:
    print("main: begin")
    # Creacion de los hilos
    p1 = threading.Thread(target=mythread, args=('A',))
    p2 = threading.Thread(target=mythread, args=('B',))
    # Se inicia la ejecucion de los hilos
    p1.start()
    p2.start()
    # El hilo padre espera que los hilos hijos culminen
    p1.join()
    p2.join()
    # time.sleep(3)
    # Mensaje que se imprime despues de que el padre reinicia su ejecucion
    print("main: end")