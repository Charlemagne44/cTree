#include "tree.h"

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
    // basewin TODO
}

void cleanup(struct ncursesObjects *objects)
{
    delwin(objects->treewin);
    delwin(objects->basewin);
    endwin();
}

void makeBoxes(struct ncursesObjects *objects)
{
    box(objects->treewin, 0, 0);
    wrefresh(objects->treewin);
}

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

struct deltas getDelta(WINDOW *win, struct branch branch)
{
    int height, width, y, x, life;
    getmaxyx(stdscr, height, width);
    y = branch.y;
    x = branch.x;
    life = branch.life;

    struct deltas returnDeltas = {0, 0};
    int xroll, yroll;
    xroll = rollDie(1, 15);
    yroll = rollDie(1, 10);

    // flags to prevent deltas from leaving the screen
    int top = FALSE;
    int bottom = FALSE;
    int left = FALSE;
    int right = FALSE;
    if (y >= height - 1)
    {
        bottom = TRUE; // TODO, PREVENT BOTTOMING OUT
    }
    else if (y <= 1)
    {
        top = TRUE;
    }

    if (x >= width - 1)
    {
        right = TRUE;
    }
    else if (x <= 1)
    {
        left = TRUE;
    }

    // TODO - Encourage growth based upon branch type
    if (life == young)
    {
        // modify dx
        if (xroll <= 5 && !right)
        {
            returnDeltas.dx++;
        }
        else if (xroll <= 10 && !left)
        {
            returnDeltas.dx--;
        }
        // modify dy - > Highly encourage young branches up
        if (yroll <= 8 && !top)
        {
            returnDeltas.dy--;
        }
    }
    else if (life == middle)
    {
        // modify dx
        if (xroll <= 7 && !right)
        {
            returnDeltas.dx++;
        }
        else if (xroll <= 15 && !left)
        {
            returnDeltas.dx--;
        }
        // modify dy
        if (yroll <= 7 && !top) // middle aged branches likely to go up
        {
            returnDeltas.dy--;
        }
    }
    else if (life == old) //
    {
        // modify dx
        if (xroll <= 5 && !right)
        {
            returnDeltas.dx++;
        }
        else if (xroll <= 10 && !left)
        {
            returnDeltas.dx--;
        }
        // modify dy
        if (yroll <= 3 && !top)
        {
            returnDeltas.dy--;
        }
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
    // wprintw(win, "Character grabbed: %c from coords: %d %d\n", character, y, x);
    if (character != ' ')
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
                // struct deltas newDelta = {i, j};
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

void grow(WINDOW *win, struct branch *branch)
{
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

    // if dead, run leaf probability and then return
    if (branch->life == dead)
    {
        if (branch->type != trunk)
        {
            // TODO - leaf logic
            branch->character = "&";
            mvwprintw(win, branch->y, branch->x, branch->character);
            wrefresh(win);
        }
        return;
    }

    int newType = getNewType(deltas, branch->type);

    // grow the branches based upon age TODO - OTHER AGES
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
        if (branchRoll <= 6) // 6/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
        }
        break;
    case old:
        if (branchRoll <= 3) // 3/10 chance to grow a young branch
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
    srand(time(0));
    grow(objects->treewin, branch);
}