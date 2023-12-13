#include "web_tree.h"

void init(struct tree *tree, int height, int width)
{
    tree->entries = malloc((height * width) * sizeof(struct entry));
}

/* Retrieve the appropriate character from the branch type */
char *getString(enum branchType type)
{
    switch (type)
    {
    case trunk:
        return "~";
    case trunkLeft:
        return "\\~";
    case trunkRight:
        return "~/";
    case left:
    case right:
        return "-";
    case leftUp:
    case rightDown:
        return "\\_";
    case leftDown:
    case rightUp:
        return "_/";
    case up:
    case down:
        return "|";
    case upLeft:
    case downRight:
        return "\\";
    case upRight:
    case downLeft:
        return "/";
    }
    return "d"; // Debug character to show something wrong with the switch
}

void addToEntries(struct tree *tree, struct branch *branch)
{
    tree->size = tree->size + 1;
    tree->entries[tree->size].character = branch->character;
    tree->entries[tree->size].y = branch->y;
    tree->entries[tree->size].x = branch->x;
}

/* Modify a deltas struct based upon the bounds and rolls given */
void modifyGrowth(struct deltas *deltas, int yroll, int xroll, int topFlag, int bottomFlag, int rightFlag, int leftFlag, int rightCap, int leftCap, int upCap, int downCap)
{
    // modify dx
    if (xroll <= rightCap && !rightFlag)
    {
        deltas->dx++;
    }
    else if (xroll <= leftCap && !leftFlag)
    {
        deltas->dx--;
    }
    // modify dy
    if (yroll <= upCap && !topFlag)
    {
        deltas->dy--;
    }
    else if (yroll <= downCap && !bottomFlag)
    {
        deltas->dy++;
    }
}

/* Check to see if a character is already printed at the new branch coordinates */
int checkCollision(struct tree *tree, int y, int x)
{
    for (int i = 0; i < tree->size; i++)
    {
        if (tree->entries[i].y == y && tree->entries[i].x == x)
        {
            if (strcmp(tree->entries[i].character, "&") == 0)
                return false;
            else if (strcmp(tree->entries[i].character, " ") == 0)
                return true;
            return false;
        }
    }
    return false;
}

/* Basic randomization with a return between upper and lower */
int rollDie(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

struct deltas *getNeighbors(struct tree *tree, int y, int x, int *n)
{
    struct deltas *collisions = NULL;
    for (int i = -1; i <= 1; i++) // will represent y deltas
    {
        for (int j = -1; j <= 1; j++) // will represent x deltas
        {
            if (i == 0 && j == 0) // don't check for collision on self
                continue;
            int collision = checkCollision(tree, y + i, x + j);
            if (collision)
            {
                *n = *n + 1;
                collisions = realloc(collisions, *n * sizeof(struct deltas));
                if (collisions == NULL)
                {
                    exit(EXIT_FAILURE);
                }
                collisions[*n - 1].dy = i;
                collisions[*n - 1].dx = j;
            }
        }
    }
    return collisions;
}

struct deltas *getFreeNeighbors(struct deltas *neighborDelta, int n)
{
    // size of free list will be 8 - size of currently found neighbors
    struct deltas *FreeDeltas = malloc((8 - n) * sizeof(struct deltas));
    int currIdx = 0;
    for (int i = -1; i <= 1; i++) // y
    {
        for (int j = -1; j <= 1; j++) // x
        {
            if (i == 0 && j == 0)
                continue;
            int inNeighors = false;
            for (int k = 0; k < n; k++)
            {
                // check to see if this delta is in the list
                if (neighborDelta[k].dy == i && neighborDelta[k].dx == j)
                {
                    inNeighors = true;
                    break;
                }
            }
            if (!inNeighors)
            {
                FreeDeltas[currIdx].dy = i;
                FreeDeltas[currIdx].dx = j;
                currIdx++;
            }
        }
    }
    return FreeDeltas;
}

/* Get the deltas for the new branch that may be created */
struct deltas getDelta(struct tree *tree, struct branch branch, int height, int width)
{
    int y, x;
    y = branch.y;
    x = branch.x;

    struct deltas returnDeltas = {0, 0};
    int xroll, yroll;
    xroll = rollDie(1, 15);
    yroll = rollDie(1, 10);

    // flags to prevent deltas from leaving the screen
    int topFlag = false;
    int bottomFlag = false;
    int leftFlag = false;
    int rightFlag = false;
    if (y >= height - 1)
    {
        bottomFlag = true;
    }
    else if (y <= 1)
    {
        topFlag = true;
    }

    if (x >= width - 1)
    {
        rightFlag = true;
    }
    else if (x <= 1)
    {
        leftFlag = true;
    }

    float heightPercentage = 1.0 - ((float)branch.y / (float)height);
    if (heightPercentage <= 0.33) // encoruage vertical growth
    {
        modifyGrowth(&returnDeltas, yroll, xroll, topFlag, bottomFlag, rightFlag, leftFlag, 4, 8, 10, 10);
    }
    else if (heightPercentage <= 0.66) // encourage balanced vertical / lateral growth
    {
        modifyGrowth(&returnDeltas, yroll, xroll, topFlag, bottomFlag, rightFlag, leftFlag, 6, 12, 6, 7);
    }
    else // encourage lateral growth
    {
        modifyGrowth(&returnDeltas, yroll, xroll, topFlag, bottomFlag, rightFlag, leftFlag, 7, 14, 3, 5);
    }

    // check for collisions
    int newy = branch.y + returnDeltas.dy;
    int newx = branch.x + returnDeltas.dx;
    if (checkCollision(tree, newy, newx))
    {
        // if the new spot will have a collision, have a chance to choose a random spot of free neighbors
        // else -> return 0, 0 to discourage more branching
        // older branches higher off the ground should have a higher probability of searching for free neighbors
        float heightMultiplier = 1.0 + (1.0 - ((float)branch.y / (float)height));
        float ageMultipier = 1.0 + (1.0 / (1 + branch.life));
        if ((float)rollDie(1, 10) * ageMultipier * heightMultiplier <= 5.0)
        {
            int n = 0;
            struct deltas *neighborDelta = getNeighbors(tree, branch.y, branch.x, &n);
            struct deltas *freeNeighbors = getFreeNeighbors(neighborDelta, n);
            int freeSize = 8 - n;
            if (freeSize)
            {
                int pickRoll = rollDie(0, freeSize - 1);
                returnDeltas.dy = freeNeighbors[pickRoll].dy;
                returnDeltas.dx = freeNeighbors[pickRoll].dx;
            }
            else
            {
                returnDeltas.dy = 0;
                returnDeltas.dx = 0;
            }
            free(neighborDelta);
            free(freeNeighbors);
        }
        else
        {
            returnDeltas.dy = 0;
            returnDeltas.dx = 0;
        }
    }

    return returnDeltas;
}

int getNewType(struct deltas deltas, enum branchType parentType)
{
    if (parentType == trunk || parentType == left || parentType == right)
    {
        if (rollDie(1, 10) <= 3)
        {
            if (deltas.dy < 0)
            {
                if (deltas.dx < 0)
                {
                    return trunkLeft;
                }
                else if (deltas.dx > 0)
                {
                    return trunkRight;
                }
                else
                {
                    return trunk;
                }
            }
        }
    }
    if (deltas.dy == 0)
    {
        if (deltas.dx == 0)
        {
            return trunk;
        }
        else if (deltas.dx < 0)
        {
            int leftRoll = rollDie(1, 10);
            if (leftRoll <= 3)
                return leftUp;
            else if (leftRoll <= 4)
                return leftDown;
            else
                return left;
        }
        else
        {
            int rightRoll = rollDie(1, 10);
            if (rightRoll <= 3)
                return rightUp;
            else if (rightRoll <= 4)
                return rightDown;
            else
                return right;
        }
    }
    else if (deltas.dy < 0)
    {
        if (deltas.dx == 0)
        {
            return up;
        }
        else if (deltas.dx < 0)
        {
            return upLeft;
        }
        else
        {
            return upRight;
        }
    }
    else
    {
        if (deltas.dx == 0)
        {
            return down;
        }
        else if (deltas.dx < 0)
        {
            return downLeft;
        }
        else
        {
            return downRight;
        }
    }
}

struct branch *createNewBranch(int life, int type, struct deltas deltas, struct branch *branch)
{
    struct branch *newBranch = malloc(sizeof(struct branch));
    newBranch->life = young;
    newBranch->type = type;
    newBranch->x = branch->x + deltas.dx;
    newBranch->y = branch->y + deltas.dy;
    newBranch->parentType = branch->type;
    newBranch->character = getString(type);

    return newBranch;
}

/* Bud leaves on eligible surrounding tiles after a branch has died within the grow function */
void bud(struct tree *tree, int y, int x, int live, long sleep_time)
{
    // character at given y x should already be a leaf, not it is our time to randomly select a set
    // of coordinates out of the surrounding 8 possible deltas to turn into a leaf
    for (int i = -1; i <= 1; i++) // y
    {
        for (int j = -1; j <= 1; j++) // x
        {
            if (i == 0 && j == 0)
                continue;
            // roll to decide if we will leaf at this coordinate
            if (rollDie(1, 10) <= 3)
            {
                int newy;
                int newx;
                newy = y + i;
                newx = x + j;
                // mvwprintw(win, newy, newx, "&");
                struct branch *newBranch = malloc(sizeof(struct branch));
                newBranch->character = "&";
                newBranch->x = newx;
                newBranch->y = newy;

                addToEntries(tree, newBranch);
                free(newBranch);
                if (live)
                    sleep(sleep_time);
            }
        }
    }
}

void grow(struct tree *tree, struct branch *branch, int live, long sleep_time, int height, int width)
{
    if (live)
    {
        sleep(sleep_time);
    }

    // render current branch and account for special 2 character branches that need to be printed with a shift
    if (branch->type == trunkLeft || branch->type == leftUp || branch->type == leftDown)
    {
        addToEntries(tree, branch);
    }
    else if (branch->type == trunkRight || branch->type == rightUp || branch->type == rightDown)
    {
        // mvwprintw(win, branch->y, branch->x, branch->character);
        branch->x = branch->x + 1;
        addToEntries(tree, branch);
    }
    else
    {
        // mvwprintw(win, branch->y, branch->x, branch->character);
        addToEntries(tree, branch);
    }

    // determine dy, and dx, and type;
    struct deltas deltas = getDelta(tree, *branch, height, width);
    if (deltas.dx == 0 && deltas.dy == 0)
    {
        branch->life = dead; // boxed in
    }

    int newType = getNewType(deltas, branch->type);

    // set height related growth parameters before budding and growing
    float heightPercentage = 1.0 - ((float)branch->y / (float)height);

    // if dead, run leaf budding and then return
    if (branch->life == dead)
    {
        // don't let a path end on a downward trend
        if (branch->type == down || branch->type == downLeft || branch->type == downRight)
        {
            // random chance to either bud or a last chance branch
            // all paths within must return
            if (rollDie(1, 10) <= 5)
            {
                struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
                grow(tree, newBranch, live, sleep_time, height, width);
                free(newBranch);
                return;
            }
            else
            {
                branch->character = "&";
                bud(tree, branch->y, branch->x, live, sleep_time);
                addToEntries(tree, branch);
                return;
            }
        }

        if (branch->type != trunk && heightPercentage > LEAF_HEIGHT_PERCENTAGE_MIN)
        {
            branch->character = "&";
            bud(tree, branch->y, branch->x, live, sleep_time);
            addToEntries(tree, branch);
        }
        return;
    }

    int branchRoll = rollDie(1, 10);
    switch (branch->life)
    {
    case young:
        if (branch->type == trunk) // young trunk -> 100% growth chance
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(tree, newBranch, live, sleep_time, height, width);
            free(newBranch);
        }
        else if (branchRoll <= 9 || heightPercentage <= LEAF_HEIGHT_PERCENTAGE_MIN) // 9/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(tree, newBranch, live, sleep_time, height, width);
            // free(newBranch);
        }
        break;
    case middle:
        if (branchRoll <= 6) // 6/10 chance to grow a middle aged branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(tree, newBranch, live, sleep_time, height, width);
            free(newBranch);
        }
        break;
    case old:
        if (branchRoll <= 3) // 3/10 chance to grow an old branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(tree, newBranch, live, sleep_time, height, width);
            free(newBranch);
        }
        break;
    case dead:
        break;
    }

    // increment age and decrement life
    branch->life++;

    // roll a chance to grow again if not dead
    if (rollDie(1, 10) <= 6 && branch->life != dead)
    {
        grow(tree, branch, live, sleep_time, height, width);
    }
}

/* Initiailze necessary parameters to begin growth, and then start the recursion */
void start(struct tree *tree, __u_long seed, int live, long sleep, int height, int width)
{

    // initalize the array of branch objects
    int n = STARTING_TRUNKS;
    struct branch *branch = malloc(n * sizeof(struct branch));
    branch->life = young;
    branch->type = trunk;
    branch->y = height - 1;
    branch->x = (width / 2);
    branch->parentType = trunk;
    branch->character = getString(branch->type);

    // recursively grow the branch, and re-render the tree each time
    srand((int)seed);
    grow(tree, branch, live, sleep, height, width);
    // free(branch);

    // print all the entries of the tree and then free it DEBUG
    printAllEntries(tree);
    free(tree);
}

void printAllEntries(struct tree *tree)
{
    for (int i = 0; i < tree->size; i++)
    {
        printf("y: %d, x: %d, string: %s\n", tree->entries[i].y, tree->entries[i].x, tree->entries->character);
    }
}

void cleanup(struct tree *tree)
{
    free(tree->entries);
}