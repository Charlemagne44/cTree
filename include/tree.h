#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#ifndef TREE_H
#define TREE_H

#define STARTING_TRUNKS 1
#define SLEEP_MILLISECONDS 50
#define SLEEP_BETWEEN_RENDER 1
#define KEY_BETWEEN_RENDER 0

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
    enum branchType parentType;
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
char getCharacter(enum branchType type);
int rollDie(int lower, int upper);
struct deltas getDelta(WINDOW *win, struct branch branch);
int getNewType(struct deltas deltas, enum branchType parentType);
int checkCollision(WINDOW *win, int y, int x);
struct deltas *getNeighbors(WINDOW *win, int y, int x, int *n);
struct deltas *getFreeNeighbors(struct deltas *neighborDeltas, int n);
struct branch *createNewBranch(int life, int type, struct deltas deltas, struct branch *branch);
void bud(WINDOW *win, int y, int x);
void grow(WINDOW *win, struct branch *branch);

// debug functions
void makeBoxes(struct ncursesObjects *objects);

#endif