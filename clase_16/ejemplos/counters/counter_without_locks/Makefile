# Contador sin lock: compila los dos ejecutables con "make"
CC     = gcc
CFLAGS = -Wall -I../include
LIBS   = -pthread
TARGS  = bad_counter counter_test

.PHONY: all clean

all: $(TARGS)

bad_counter: counter_without_locks.c counter.c counter.h ../include/common.h
	$(CC) $(CFLAGS) counter_without_locks.c counter.c $(LIBS) -o $@

counter_test: test_counter_cli.c counter.c counter.h ../include/common.h
	$(CC) $(CFLAGS) test_counter_cli.c counter.c $(LIBS) -o $@

clean:
	rm -f $(TARGS)
