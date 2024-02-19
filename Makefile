CC=gcc
CFLAGS=-I.
mymalloc.o: mymalloc.h mymalloc.c
	$(CC) -g -c mymalloc.c
memtest.o: memtest.c mymalloc.h
	$(CC) -g -c memtest.c
memtest: mymalloc.o memtest.o
	$(CC) -g mymalloc.o memtest.o -o memtest 
memgrind.o: memgrind.c mymalloc.h
	$(CC) -g -c memgrind.c 
memgrind: memgrind.o mymalloc.o
	$(CC) -g memgrind.o mymalloc.o -o memgrind 
all: memtest memgrind 
clean:
	rm -f *.o memtest memgrind 