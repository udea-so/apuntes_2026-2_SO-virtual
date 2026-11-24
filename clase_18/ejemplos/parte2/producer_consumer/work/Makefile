# Codigo original de OSTEP (ostep-code/threads-sema/producer_consumer_works.c)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all clean

all: producer_consumer_works.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
