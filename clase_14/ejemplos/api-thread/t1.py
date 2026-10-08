import threading
import sys

# Necesario usar un objeto para que sea mutable y compartido entre hilos
class Counter:
    def __init__(self):
        self.value = 0

counter = Counter()
max = 0

def mythread(arg):
  global counter
  # stack (private per thread)   
  print(f"{arg}: begin")
  for i in range(max):    
    counter.value += 1
  print(f"{arg}: done")

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("usage: t1.py <loopcount>")
  else:
    max = int(sys.argv[1])    
    print(f"main: begin [counter = {counter.value}]")
  
    # Creacion de los hilos
    p1 = threading.Thread(target=mythread, args=('A',))
    p2 = threading.Thread(target=mythread, args=('B',))
    # Se inicia la ejecucion de los hilos
    p1.start()
    p2.start()
    # El hilo padre espera que los hilos hijos culminen
    p1.join()
    p2.join()
    # Mensaje que se imprime despues de que el padre reinicia su ejecucion
    # print(f"end: begin [counter = {counter.value}] [address = {hex(id(counter))}]")
    print(f"main: done")
    print(f" [counter: {counter.value}]\n [should: {max*2}]")