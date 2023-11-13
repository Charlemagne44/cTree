#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#define STARTING_TRUNKS 1

enum life
{
    young,
    middle,
    old,
    dead
};

enum branchType
{
    trunk,
    left,
    right,
    up,
    upLeft,
    upRight,
    down,
    downLeft,
    downRight,
};

struct branch
{
    enum branchType type;
    enum life life;
    int x, y;
    char *character;
};

struct ncursesObjects
{
    WINDOW *treewin;
    WINDOW *basewin;
};

struct deltas
{
    int dy;
    int dx;
};

// necessary functions
void init(struct ncursesObjects *objects);
void cleanup(struct ncursesObjects *objects);
void start(struct ncursesObjects *objects);
char getCharacter(struct branch branch);
int rollDie(int lower, int upper);
struct deltas getDelta(WINDOW *win, struct branch branch);
int getNewType(struct deltas deltas);
int checkCollision(WINDOW *win, int y, int x);
struct deltas *getNeighbors(WINDOW *win, int y, int x);
void grow(WINDOW *win, struct branch *branch);

// debug functions
void makeBoxes(struct ncursesObjects *objects);