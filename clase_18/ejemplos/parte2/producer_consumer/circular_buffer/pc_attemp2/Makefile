# Mutex alrededor de todo el ciclo: deadlock (diapositivas p. 17-21)
#   make         compila pc_attemp2.out
#   make traza   compila ademas pc_attemp2-traza.out (-DTRAZA)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h verifica.h

.PHONY: all clean traza

all: pc_attemp2.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

traza: pc_attemp2-traza.out

pc_attemp2-traza.out: pc_attemp2.c $(HEADERS)
	$(CC) $(CFLAGS) -DTRAZA -o $@ $<

clean:
	rm -f *.out *.o
