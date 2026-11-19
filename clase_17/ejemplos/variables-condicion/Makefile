# Compila cada archivo .c de la carpeta en un ejecutable .out
CC          = gcc
CFLAGS      = -Wall -I. -pthread
SOURCES     = $(wildcard *.c)
EXECUTABLES = $(SOURCES:.c=.out)

.PHONY: all clean

all: $(EXECUTABLES)

%.out: %.c mythreads.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLES)
