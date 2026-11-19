# Semáforo binario como lock (diapositivas p. 13-17)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all clean

all: binary.out binary-sin-semaforo.out

binary.out: binary.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

# Variante sin sem_wait()/sem_post(): muestra la condición de carrera
binary-sin-semaforo.out: binary.c $(HEADERS)
	$(CC) $(CFLAGS) -DSIN_SEMAFORO -o $@ $<

clean:
	rm -f *.out *.o
