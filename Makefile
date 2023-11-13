all:
	gcc tree.c -lncurses -Wall -o tree

clean:
	rm tree