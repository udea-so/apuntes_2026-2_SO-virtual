# Comparacion preciso vs. aproximado: compila con "make"
CC     = gcc
CFLAGS = -Wall -I../include
LIBS   = -pthread
TARG   = run_benchmark
SRCS   = benchmark_counters.c precise_counter.c sloppy_counter.c

.PHONY: all clean

all: $(TARG)

$(TARG): $(SRCS) precise_counter.h sloppy_counter.h ../include/common.h
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $@

clean:
	rm -f $(TARG)
