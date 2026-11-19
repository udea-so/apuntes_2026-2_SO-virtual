# Semáforo para ordenar la ejecución: el padre espera al hijo (diapositivas p. 21-26)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all traza clean

all: sem_as_CV.out

sem_as_CV.out: sem_as_CV.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

# Variante que imprime el valor del semáforo (sem_getvalue) en cada paso
traza: sem_as_CV-traza.out

sem_as_CV-traza.out: sem_as_CV.c $(HEADERS)
	$(CC) $(CFLAGS) -DTRAZA -o $@ $<

clean:
	rm -f *.out *.o
