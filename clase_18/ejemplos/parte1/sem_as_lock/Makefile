# Lock implementado con un semáforo (diapositivas p. 18-20)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all clean

all: binary.out test_lock.out

# binary.c usa el lock_t de my_lock.c (p. 19)
binary.out: binary.c my_lock.c my_lock.h $(HEADERS)
	$(CC) $(CFLAGS) -o $@ binary.c my_lock.c

# Código de test de la p. 20
test_lock.out: test_lock.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.out *.o
