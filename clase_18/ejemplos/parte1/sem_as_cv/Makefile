# Señalización padre-hijo con la interfaz my_cv (semáforo en 0)
CC      = gcc
CFLAGS  = -Wall -I. -pthread
HEADERS = common.h common_threads.h

.PHONY: all clean

all: sem_as_CV.out

sem_as_CV.out: sem_as_CV.c my_cv.c my_cv.h $(HEADERS)
	$(CC) $(CFLAGS) -o $@ sem_as_CV.c my_cv.c

clean:
	rm -f *.out *.o
