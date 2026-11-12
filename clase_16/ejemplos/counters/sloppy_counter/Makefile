# Contador aproximado: compila los tres ejecutables con "make"
CC     = gcc
CFLAGS = -Wall -I../include
LIBS   = -pthread
TARGS  = sloppy_example benchmark_sloppy benchmark_thresh
DEPS   = sloppy_counter.c sloppy_counter.h ../include/common.h

.PHONY: all clean

all: $(TARGS)

sloppy_example: main_sloppy_counter.c $(DEPS)
	$(CC) $(CFLAGS) main_sloppy_counter.c sloppy_counter.c $(LIBS) -o $@

benchmark_sloppy: benchmark_sloppy_counter.c $(DEPS)
	$(CC) $(CFLAGS) benchmark_sloppy_counter.c sloppy_counter.c $(LIBS) -o $@

benchmark_thresh: benchmark_thresh_amt1.c $(DEPS)
	$(CC) $(CFLAGS) benchmark_thresh_amt1.c sloppy_counter.c $(LIBS) -o $@

clean:
	rm -f $(TARGS)
