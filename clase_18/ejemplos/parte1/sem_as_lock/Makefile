CC     := gcc
CFLAGS := -Wall -Werror -pthread

# Detectar sistema operativo
OS     := $(shell uname -s)
LIBS   := 
ifeq ($(OS),Linux)
	LIBS += -pthread
endif

# Objetivo por defecto
all: binary

# 1. Linkeo final: une binary.o y my_lock.o
binary: binary.o my_lock.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# 2. Compila el main (depende de common headers y my_lock.h)
binary.o: binary.c my_lock.h common_threads.h common.h
	$(CC) $(CFLAGS) -c binary.c

# 3. Compila la implementación del lock
my_lock.o: my_lock.c my_lock.h common_threads.h
	$(CC) $(CFLAGS) -c my_lock.c

clean:
	rm -f binary *.o