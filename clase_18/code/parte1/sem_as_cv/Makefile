CC     := gcc
CFLAGS := -Wall -Werror -pthread

# Detección del Sistema Operativo (Mantenemos tu lógica original)
OS     := $(shell uname -s)
LIBS   := 
ifeq ($(OS),Linux)
	LIBS += -pthread
endif

# Nombre del ejecutable final
TARGET := sem_as_CV

# Lista de objetos necesarios (Main + tu librería)
OBJS   := sem_as_CV.o my_cv.o

.PHONY: all clean

all: $(TARGET)

# Regla de Linkeo: Junta todos los .o para crear el ejecutable
$(TARGET): $(OBJS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Regla de Compilación: Convierte cada .c en .o
# Agregamos los headers como dependencias para recompilar si cambian
%.o: %.c my_cv.h common_threads.h common.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)