# Definición de variables
CC = gcc                         # Compilador
CFLAGS = -Wall -pthread          # Opciones de compilación (avisos, hilos)
SOURCES = $(wildcard *.c)        # Todos los archivos .c en el directorio actual
EXECUTABLES = $(SOURCES:.c=.out) # Ejecutables generados, uno por cada archivo fuente
INCLUDES = .

# Encuentra todos los archivos .c en el directorio actual
SOURCES = $(wildcard *.c)

# Genera una lista de ejecutables, cambiando la extensión .c a .out
EXECUTABLES = $(SOURCES:.c=.out)

# Regla principal (por defecto)
all: $(EXECUTABLES)

# Regla para compilar cada archivo .c en un archivo .out
%.out: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Regla de limpieza
clean:
	rm -f $(EXECUTABLES)