#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifndef TREE_H
#define TREE_H

#define STARTING_TRUNKS 1
#define KEY_BETWEEN_RENDER 0
#define LEAF_HEIGHT_PERCENTAGE_MIN 0.30
#define INFINITE_WAIT_SECONDS_BREAK 3

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

struct deltas
{
    int dy;
    int dx;
};

struct tree
{
    struct entry *entries;
    int size;
};

struct entry
{
    int y;
    int x;
    char *character;
};

// web specific functions
void addToEntries(struct tree *tree, struct branch *branch);
void printAllEntries(struct tree *tree);

// necessary functions
void init(struct tree *tree, int height, int width);
void cleanup(struct tree *tree);
void start(struct tree *tree, __u_long seed, int live, long sleep, int height, int width);
char *getString(enum branchType type);
int rollDie(int lower, int upper);
struct deltas getDelta(struct tree *tree, struct branch branch, int height, int width);
void modifyGrowth(struct deltas *deltas, int yroll, int xroll, int topFlag, int bottomFlag, int rightFlag, int leftFlag, int rightCap, int leftCap, int upCap, int downCap);
int getNewType(struct deltas deltas, enum branchType parentType);
int checkCollision(struct tree *tree, int y, int x);
struct deltas *getNeighbors(struct tree *tree, int y, int x, int *n);
struct deltas *getFreeNeighbors(struct deltas *neighborDelta, int n);
struct branch *createNewBranch(int life, int type, struct deltas deltas, struct branch *branch);
void bud(struct tree *tree, int y, int x, int live, long sleep_time);
void grow(struct tree *tree, struct branch *branch, int live, long sleep_time, int height, int width);

#endif