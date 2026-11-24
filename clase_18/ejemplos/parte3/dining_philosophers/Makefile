# Filosofos comensales (diapositivas p. 35-46)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all clean

all: dining_philosophers_deadlock.out dining_philosophers_deadlock_print.out      dining_philosophers_no_deadlock.out dining_philosophers_no_deadlock_print.out

%.out: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
