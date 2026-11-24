# Lock de lectores-escritores (diapositivas p. 26-34)
#   make         compila rwlock.out y rwlock-lectura-larga.out
#   make clean   borra los ejecutables
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all clean

all: rwlock.out rwlock-lectura-larga.out

rwlock.out: rwlock.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

# Variante con lecturas largas: muestra la inanicion de los escritores (p. 34)
rwlock-lectura-larga.out: rwlock.c $(HEADERS)
	$(CC) $(CFLAGS) -DLECTURA_LARGA -o $@ $<

clean:
	rm -f *.out *.o
