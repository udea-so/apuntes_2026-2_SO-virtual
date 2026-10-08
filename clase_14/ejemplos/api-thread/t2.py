import threading

# Equvalente a struct en C
class MyArg:
    def __init__(self, a, b):
        self.a = a
        self.b = b

class MyRet:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

# ----------------------------------------
# Funcion que ejecutan los hilos
# ----------------------------------------

def mythread(arg, result_container):
    print(arg.a, arg.b)    
    result = MyRet(x=1, y=2)
    """
    En Python, thread.join() no retorna un valor. Una forma común
    de "retornar" un valor es pasar un objeto mutable (como un 
    diccionario o una lista) donde el hilo pueda depositar su resultado.
    """
    result_container.append(result) 

# ----------------------------------------
# Hilo principal
# ----------------------------------------
if __name__ == "__main__":
  myarg_t = MyArg(10, 20)
  # Contenedor compartido para recibir el resultado
  result_container = []

  # Creación del hilo
  p = threading.Thread(target=mythread, args=(myarg_t, result_container))
  p.start()
  p.join()  # Se espera a que el hilo termine
  
  # Se obtiene el resultado del contenedor
  m = result_container[0]  
  print(f"main: result = {m.x}, {m.y}")

  