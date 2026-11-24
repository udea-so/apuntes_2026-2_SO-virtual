# Mutex solo alrededor de put/get: solucion correcta (diapositivas p. 22-25)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h verifica.h

.PHONY: all clean

all: pc_work.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
