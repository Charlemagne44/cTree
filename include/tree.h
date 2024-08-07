#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <panel.h>
#include <time.h>

#ifndef TREE_H
#define TREE_H

#define STARTING_TRUNKS 1
#define KEY_BETWEEN_RENDER 0
#define LEAF_HEIGHT_PERCENTAGE_MIN 0.30
#define INFINITE_WAIT_MILLSECONDS_BREAK 3000

typedef unsigned long __u_long;

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
    trunkLeft,
    trunkRight,
    left,
    leftUp,
    leftDown,
    right,
    rightUp,
    rightDown,
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
    WINDOW *helpwin;
    WINDOW *debugwin;
    PANEL *treePanel;
    PANEL *basePanel;
    PANEL *helpPanel;
    PANEL *debugPanel;
};

struct deltas
{
    int dy;
    int dx;
};

// necessary functions
void init(struct ncursesObjects *objects);
void cleanup(struct ncursesObjects *objects);
void start(struct ncursesObjects *objects, __u_long seed, int live, long sleep);
char *getString(enum branchType type);
int rollDie(int lower, int upper);
struct deltas getDelta(WINDOW *win, struct branch branch);
void modifyGrowth(struct deltas *deltas, int yroll, int xroll, int topFlag, int bottomFlag, int rightFlag, int leftFlag, int rightCap, int leftCap, int upCap, int downCap);
int getNewType(struct deltas deltas, enum branchType parentType);
int checkCollision(WINDOW *win, int y, int x);
struct deltas *getNeighbors(WINDOW *win, int y, int x, int *n);
struct deltas *getFreeNeighbors(struct deltas *neighborDeltas, int n);
struct branch *createNewBranch(int life, int type, struct deltas deltas, struct branch *branch);
void bud(WINDOW *win, int y, int x, int live, long sleep);
void grow(WINDOW *win, struct branch *branch, int live, long sleep);

// debug functions
void makeBoxes(struct ncursesObjects *objects);
void printTimeSeed(struct ncursesObjects *objects, time_t seed);
void printHelp();

#endif