CC = gcc
CFLAGS = -Wall -pedantic -std=c99
LDFLAGS = -lcurses

SOURCE_FILES = 

.PHONY = clean

debugger:
	$(CC) $(CFLAGS) main.c $(SOURCE_FILES) -o $@ $(LDFLAGS)

clean:
	rm debugger
	rm -f *.o
