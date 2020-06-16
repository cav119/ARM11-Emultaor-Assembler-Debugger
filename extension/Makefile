CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -g
LDFLAGS = -lcurses -lpthread

SOURCE_FILES = gui.c str_list.c

.PHONY = clean

debugger:
	$(CC) $(CFLAGS) main.c $(SOURCE_FILES) -o $@ $(LDFLAGS)

clean:
	rm debugger
	rm -f *.o
