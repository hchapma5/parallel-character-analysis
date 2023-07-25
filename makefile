COMPILER = gcc
CFLAGS = -Wall -g -pedantic
EXES = char_count
all: ${EXES}

ringlib: ringlib.c
	${COMPILER} ${CFLAGS} ringlib.c 

charlib: charlib.c
	${COMPILER} ${CFLAGS} charlib.c

char_count: char_count.c ringlib.o charlib.o
	${COMPILER} ${CFLAGS} char_count.c ringlib.o charlib.o -o char_count

clean:
	rm -f *.o ${EXES}


