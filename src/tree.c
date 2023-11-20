#include "tree.h"

/* All ncurses related initialization */
void init(struct ncursesObjects *objects)
{
    // ncurses initialization
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    // window initialization
    int height, width;
    getmaxyx(stdscr, height, width);
    objects->treewin = newwin(height, width, 0, 0);
}

/* Ncurses objects cleanup */
void cleanup(struct ncursesObjects *objects)
{
    delwin(objects->treewin);
    delwin(objects->basewin);
    endwin();
}

/* Make boxes around the ncurses objects for visualization */
void makeBoxes(struct ncursesObjects *objects)
{
    box(objects->treewin, 0, 0);
    wrefresh(objects->treewin);
}

/* Print the time seed to assist in debugging from time based randomization */
void printTimeSeed(WINDOW *win, time_t seed)
{
    mvwprintw(win, 0, 0, "Time seed: %ld\n", seed);
}

/* Retrieve the appropriate character from the branch type */
char getCharacter(enum branchType type)
{
    switch (type)
    {
    case trunk:
        return '~';
    case left:
    case right:
        return '-';
    case up:
    case down:
        return '|';
    case upLeft:
    case downRight:
        return '\\';
    case upRight:
    case downLeft:
        return '/';
    }
    return 'd'; // Debug character to show something wrong with the switch
}

/* Basic randomization with a return between upper and lower */
int rollDie(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

/* Given a list of currently occupied neighbors, return a list of all the remaining free deltas */
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
            int inNeighors = FALSE;
            for (int k = 0; k < n; k++)
            {
                // check to see if this delta is in the list
                if (neighborDelta[k].dy == i && neighborDelta[k].dx == j)
                {
                    inNeighors = TRUE;
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

/* Get the deltas for the new branch that may be created */
struct deltas getDelta(WINDOW *win, struct branch branch)
{
    int height, width, y, x;
    getmaxyx(stdscr, height, width);
    y = branch.y;
    x = branch.x;

    struct deltas returnDeltas = {0, 0};
    int xroll, yroll;
    xroll = rollDie(1, 15);
    yroll = rollDie(1, 10);

    // flags to prevent deltas from leaving the screen
    int topFlag = FALSE;
    int bottomFlag = FALSE;
    int leftFlag = FALSE;
    int rightFlag = FALSE;
    if (y >= height - 1)
    {
        bottomFlag = TRUE;
    }
    else if (y <= 1)
    {
        topFlag = TRUE;
    }

    if (x >= width - 1)
    {
        rightFlag = TRUE;
    }
    else if (x <= 1)
    {
        leftFlag = TRUE;
    }

    // TODO - Encourage growth based upon branch type, height, and age
    float heightPercentage = 1.0 - ((float)branch.y / (float)height);
    if (heightPercentage <= 0.33) // encoruage vertical growth
    {
        modifyGrowth(&returnDeltas, yroll, xroll, topFlag, bottomFlag, rightFlag, leftFlag, 4, 8, 9, 9);
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
    if (checkCollision(win, newy, newx))
    {
        // if the new spot will have a collision, have a chance to choose a random spot of free neighbors
        // else -> return 0, 0 to discourage more branching
        // older branches higher off the ground should have a higher probability of searching for free neighbors
        // TODO - ENCOURAGE CURRENT TREND OF GROWTH
        // TODO - PREVENT BOTTOMING OUT
        int heightMultiplier = 1.0 + (1.0 - ((float)branch.y / (float)height));
        int ageMultipier = 1.0 + (1.0 / (1 + branch.life));
        if (rollDie(1, 10) * ageMultipier * heightMultiplier <= 5.0)
        {
            int n = 0;
            struct deltas *neighborDelta = getNeighbors(win, branch.y, branch.x, &n);
            struct deltas *freeNeighbors = getFreeNeighbors(neighborDelta, n);
            int freeSize = 8 - n;
            int pickRoll = rollDie(0, freeSize - 1);
            returnDeltas.dy = freeNeighbors[pickRoll].dy;
            returnDeltas.dx = freeNeighbors[pickRoll].dx;
        }
        else
        {
            returnDeltas.dy = 0;
            returnDeltas.dx = 0;
        }
    }

    return returnDeltas;
}

/* Get the type of the new branch with a little randomization included */
int getNewType(struct deltas deltas, enum branchType parentType)
{
    // TODO - create a random chance of another trunk occuring, which has unique growth patterns
    if (parentType == trunk || parentType == left || parentType == right)
    {
        if (rollDie(1, 10) <= 3)
        {
            return trunk;
        }
    }
    if (deltas.dy == 0)
    {
        if (deltas.dx == 0)
        {
            return trunk; // TODO - impossible , will not happen
        }
        else if (deltas.dx < 0)
        {
            return left;
        }
        else
        {
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

/* Check to see if a character is already printed at the new branch coordinates */
int checkCollision(WINDOW *win, int y, int x)
{
    chtype ch = mvwinch(win, y, x);
    char character = (char)(ch & A_CHARTEXT);
    if (character == '&')
        return FALSE;
    else if (character != ' ')
    {
        return TRUE;
    }
    return FALSE;
}

/* Check all 8 surrounding coordinates to see if there are any collions that could happen from branching */
struct deltas *getNeighbors(WINDOW *win, int y, int x, int *n)
{
    struct deltas *collisions = NULL;
    for (int i = -1; i <= 1; i++) // will represent y deltas
    {
        for (int j = -1; j <= 1; j++) // will represent x deltas
        {
            if (i == 0 && j == 0) // don't check for collision on self
                continue;
            int collision = checkCollision(win, y + i, x + j);
            if (collision)
            {
                *n = *n + 1;
                collisions = realloc(collisions, *n * sizeof(struct deltas));
                if (collisions == NULL)
                {
                    endwin();
                    exit(EXIT_FAILURE);
                }
                collisions[*n - 1].dy = i;
                collisions[*n - 1].dx = j;
            }
        }
    }
    return collisions;
}

struct branch *createNewBranch(int life, int type, struct deltas deltas, struct branch *branch)
{
    struct branch *newBranch = malloc(sizeof(struct branch));
    newBranch->life = young;
    newBranch->type = type;
    newBranch->x = branch->x + deltas.dx;
    newBranch->y = branch->y + deltas.dy;
    newBranch->parentType = branch->type;
    char newStr[2];
    newStr[0] = getCharacter(type);
    newStr[1] = '\0';
    newBranch->character = newStr;

    return newBranch;
}

/* Bud leaves on eligible surrounding tiles after a branch has died within the grow function */
void bud(WINDOW *win, int y, int x)
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
                mvwprintw(win, newy, newx, "&");
                wrefresh(win);
                if (SLEEP_BETWEEN_RENDER)
                    napms(SLEEP_MILLISECONDS);
            }
        }
    }
}

/* Main recursive growth functino kicked off from start */
void grow(WINDOW *win, struct branch *branch)
{
    // either wait for input or sleep depending on config
    if (KEY_BETWEEN_RENDER)
        getch();
    else if (SLEEP_BETWEEN_RENDER)
    {
        napms(SLEEP_MILLISECONDS);
    }

    // render current branch;
    mvwprintw(win, branch->y, branch->x, branch->character);
    wrefresh(win);

    // determine dy, and dx, and type;
    struct deltas deltas = getDelta(win, *branch);
    if (deltas.dx == 0 && deltas.dy == 0)
    {
        branch->life = dead; // boxed in
    }

    int newType = getNewType(deltas, branch->type);

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
                grow(win, newBranch);
                return;
            }
            else
            {
                branch->character = "&";
                mvwprintw(win, branch->y, branch->x, branch->character);
                wrefresh(win);
                return;
            }
        }
        int height = getmaxy(win);
        float heightPercentage = 1.0 - ((float)branch->y / (float)height);
        if (branch->type != trunk && heightPercentage > LEAF_HEIGHT_PERCENTAGE_MIN)
        {
            branch->character = "&";
            bud(win, branch->y, branch->x);
            mvwprintw(win, branch->y, branch->x, branch->character);
            wrefresh(win);
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
            grow(win, newBranch);
        }
        else if (branchRoll <= 9) // 9/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
        }
        break;
    case middle:
        if (branchRoll <= 6) // 6/10 chance to grow a middle aged branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
        }
        break;
    case old:
        if (branchRoll <= 3) // 3/10 chance to grow an old branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
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
        grow(win, branch);
    }
}

/* Initiailze necessary parameters to begin growth, and then start the recursion */
void start(struct ncursesObjects *objects)
{
    // get screen metrics
    int height, width;
    getmaxyx(stdscr, height, width);

    // initalize the array of branch objects
    int n = STARTING_TRUNKS;
    struct branch *branch = malloc(n * sizeof(struct branch));
    branch->life = young;
    branch->type = trunk;
    branch->y = height - 1;
    branch->x = (width / 2);
    branch->parentType = trunk;

    char str[2];
    str[0] = getCharacter(branch->type);
    str[1] = '\0';
    branch->character = str;

    // recursively grow the branch, and re-render the tree each time
    time_t seed = time(0);
    srand(seed);
    printTimeSeed(objects->treewin, seed);
    grow(objects->treewin, branch);
}