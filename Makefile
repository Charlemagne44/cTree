CC = gcc
CFLAGS = -Wall
LIBS = -lncurses
INCLUDE = -I./include

all: tree test

tree: src/tree.c
	$(CC) $(CFLAGS) $< -o bin/$@ -c $(LIBS) $(INCLUDE)

test: testing/test.c tree
	$(CC) $(CFLAGS) $< -o bin/$@ bin/tree $(LIBS) $(INCLUDE)

clean:
	rm -f bin/* *.o
