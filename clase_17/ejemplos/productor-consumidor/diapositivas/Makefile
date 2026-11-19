# Compila cada archivo .c de la carpeta en un ejecutable .out
#   make         compila todas las versiones
#   make traza   compila ademas v1 y v2 con la traza de pasos (-DTRAZA)
#   make clean   borra los ejecutables
CC          = gcc
CFLAGS      = -Wall -I. -pthread
SOURCES     = $(wildcard *.c)
EXECUTABLES = $(SOURCES:.c=.out)
TRAZAS      = main_singleBuffer-v1-traza.out main_singleBuffer-v2-traza.out

.PHONY: all traza clean

all: $(EXECUTABLES)

traza: $(TRAZAS)

%.out: %.c mythreads.h
	$(CC) $(CFLAGS) -o $@ $<

%-traza.out: %.c mythreads.h
	$(CC) $(CFLAGS) -DTRAZA -o $@ $<

clean:
	rm -f $(EXECUTABLES) $(TRAZAS)
