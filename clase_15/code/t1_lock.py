import threading
import sys


# python3 t1_lock.py 1000

# Necesario usar un objeto para que sea mutable y compartido entre hilos
class Counter:
    def __init__(self):
        self.value = 0

counter = Counter()
lock = threading.Lock() # Crear un lock global
max = 0

def mythread(arg):
  global counter
  # stack (private per thread)   
  print(f"{arg}: begin")
  for i in range(max):
    with lock:               # Context manager: adquiere y libera el lock automaticamente
       # --- INICIO DE LA SECCIÓN CRÍTICA ---
       counter.value += 1
       # --- FIN DE LA SECCIÓN CRÍTICA ---
    # Simular a las llamadas pthread_mutex_lock y pthread_mutex_unlock
    # lock.acquire()          # pthread_mutex_lock()    
    # counter.value += 1
    # lock.release()          # pthread_mutex_unlock()
    
  print(f"{arg}: done")

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("usage: main <loopcount>")
  else:
    max = int(sys.argv[1])
    print("main: begin")
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
    print(f"end: begin [counter = {counter.value}]")
    print(f" [counter: {counter.value}]\n [should: {max*2}]\n")