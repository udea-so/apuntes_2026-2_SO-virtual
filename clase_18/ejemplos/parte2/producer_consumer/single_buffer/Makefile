# Productor/consumidor con buffer de un elemento (diapositivas p. 10-12)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h verifica.h

.PHONY: all clean

all: pc_single.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
