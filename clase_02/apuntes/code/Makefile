all: cpu threads io mem

cpu: cpu.c
	gcc -o cpu cpu.c -Wall

threads: threads.c
	gcc -o threads threads.c -Wall -pthread

io: io.c
	gcc -o io io.c -Wall

mem: mem.c
	gcc -o mem mem.c -Wall

clean:
	rm -f cpu threads io mem
