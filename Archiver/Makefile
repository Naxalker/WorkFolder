CC = gcc -fsanitize=address
FLAGS = -c -Wall -Wextra 

all: archiver.o
	$(CC) archiver.o -o build/arch

archiver.o: archiver.c archiver.h
	$(CC) $(FLAGS) archiver.c

clean:
	rm -rf *.o build/*