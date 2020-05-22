CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o
	gcc assemble.o -o assemble
emulate: emulate.o
	gcc emulate.c -o emulate
clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
