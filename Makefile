CC = gcc
CFLAGS = -Wall
DEBUGFLAGS = -g
LIBS = -lncurses -lpanel -std=gnu11
INCLUDE = -I./include

SRC_DIR = src
TEST_DIR = testing
BIN_DIR = bin

all: main test

main: $(SRC_DIR)/main.c $(SRC_DIR)/tree.c
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/ctree $(LIBS) $(INCLUDE)

test: $(TEST_DIR)/test.c $(SRC_DIR)/tree.c
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$@ $(LIBS) $(INCLUDE)

debug: $(SRC_DIR)/main.c $(SRC_DIR)/tree.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $^ -o $(BIN_DIR)/$@ $(LIBS) $(INCLUDE)

clean:
	rm -f $(BIN_DIR)/* *.o