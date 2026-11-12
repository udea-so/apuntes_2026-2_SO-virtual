# Contador preciso: compila los dos ejecutables con "make"
CC     = gcc
CFLAGS = -Wall -I../include
LIBS   = -pthread
TARGS  = precise_counter benchmark_precise_counter

.PHONY: all clean

all: $(TARGS)

precise_counter: main_precise_counter.c counter_lock.c counter_lock.h ../include/common.h
	$(CC) $(CFLAGS) main_precise_counter.c counter_lock.c $(LIBS) -o $@

benchmark_precise_counter: benchmark_precise_counter.c counter_lock.c counter_lock.h ../include/common.h
	$(CC) $(CFLAGS) benchmark_precise_counter.c counter_lock.c $(LIBS) -o $@

clean:
	rm -f $(TARGS)
