# Buffer circular sin mutex: race condition (diapositivas p. 13-16)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h verifica.h

.PHONY: all clean

all: pc_attemp1.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
