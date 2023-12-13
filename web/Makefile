# build
CC = gcc
CFLAGS = -Wall
DEBUGFLAGS = -g
LIBS = -std=gnu11
INCLUDE = -I./include

SRC_DIR = src
TEST_DIR = testing
BIN_DIR = bin

# installation
PREFIX = /usr/local/bin

all: main

main: $(SRC_DIR)/web_main.c $(SRC_DIR)/web_tree.c
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/web_ctree $(LIBS) $(INCLUDE)

debug: $(SRC_DIR)/web_main.c $(SRC_DIR)/web_tree.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $^ -o $(BIN_DIR)/debug_web_ctree $(LIBS) $(INCLUDE)

clean:
	rm -f $(BIN_DIR)/* *.o