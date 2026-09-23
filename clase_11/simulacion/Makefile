# Makefile - Laboratorio: Medicion empirica de la TLB (Saavedra-Barrera)
#
# Targets:
#   make            -> compila tlb_bench
#   make run        -> compila (si hace falta) y corre el benchmark, genera resultados.csv
#   make plot       -> genera resultados.csv (si no existe) y la grafica resultados.png
#   make clean      -> borra binario, CSV y PNG generados

CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c11
BIN     = tlb_bench
CSV     = resultados.csv
PNG     = resultados.png
PYTHON  = python3

# Parametros del barrido. Ajustar segun la RAM disponible en la maquina.
# Con paginas de 4KB, 65536 paginas = 256MB.
PAGINAS_MIN = 1
PAGINAS_MAX = 65536
REPETICIONES = 7
# CPU al que se fija el proceso (pregunta R6 del homework). -1 = sin pinning.
CPU = 0
# Accesos minimos por pasada cronometrada, para diluir el costo del timer
# (pregunta R1 del homework).
ACCESOS_OBJETIVO = 200000

.PHONY: all run plot clean

all: $(BIN)

$(BIN): tlb_bench.c
	$(CC) $(CFLAGS) $< -o $@

run: $(BIN)
	./$(BIN) -m $(PAGINAS_MIN) -M $(PAGINAS_MAX) -r $(REPETICIONES) \
	          -c $(CPU) -a $(ACCESOS_OBJETIVO) -o $(CSV)

plot: run
	$(PYTHON) plot_tlb.py $(CSV) $(PNG)

clean:
	rm -f $(BIN) $(CSV) $(PNG)
