# Zemaforos: semaforo con lock y variable de condicion (diapositivas p. 47-48)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h zemaphore.h

.PHONY: all clean

all: zem_test.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
