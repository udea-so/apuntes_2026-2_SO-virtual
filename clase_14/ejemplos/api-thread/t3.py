import threading

# Equivalente a t3.c: se pasa un valor simple (un numero) al hilo,
# sin clase que lo empaquete (en C: sin struct ni malloc), y el hilo
# devuelve otro valor simple.

# ----------------------------------------
# Funcion que ejecutan los hilos
# ----------------------------------------

def mythread(value, result_container):
    print(value)
    """
    Como thread.join() no retorna un valor (en C, pthread_join si lo
    entrega), el hilo deposita su resultado en un objeto mutable
    compartido, igual que en t2.py.
    """
    result_container.append(value + 1)

# ----------------------------------------
# Hilo principal
# ----------------------------------------
if __name__ == "__main__":
  # Contenedor compartido para recibir el resultado
  result_container = []

  # Creación del hilo: el numero va directamente en args.
  # En Python no hay punteros ni tamanos de tipo que coordinar:
  # no existe el error de la version de las diapositivas de t3.c
  p = threading.Thread(target=mythread, args=(100, result_container))
  p.start()
  p.join()  # Se espera a que el hilo termine

  rvalue = result_container[0]
  print(f"returned {rvalue}")
