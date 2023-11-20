CC = gcc
CFLAGS = -Wall
LIBS = -lncurses -lpanel
INCLUDE = -I./include

SRC_DIR = src
TEST_DIR = testing
BIN_DIR = bin

all: main test

main: $(SRC_DIR)/main.c $(SRC_DIR)/tree.c
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$@ $(LIBS) $(INCLUDE)

test: $(TEST_DIR)/test.c $(SRC_DIR)/tree.c
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$@ $(LIBS) $(INCLUDE)

clean:
	rm -f $(BIN_DIR)/* *.o