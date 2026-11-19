# Ensayos con wait y signal: un ejecutable por variante de test1.c
CC     = gcc
CFLAGS = -Wall -I. -pthread

EXECUTABLES = test1.out test1-sin-signal.out test1-doble-signal.out test1-wait-en-main.out

.PHONY: all clean

all: $(EXECUTABLES)

test1.out: test1.c mythreads.h
	$(CC) $(CFLAGS) -o $@ $<

test1-sin-signal.out: test1.c mythreads.h
	$(CC) $(CFLAGS) -DSIN_SIGNAL -o $@ $<

test1-doble-signal.out: test1.c mythreads.h
	$(CC) $(CFLAGS) -DDOBLE_SIGNAL -o $@ $<

test1-wait-en-main.out: test1.c mythreads.h
	$(CC) $(CFLAGS) -DWAIT_EN_MAIN -o $@ $<

clean:
	rm -f $(EXECUTABLES)
