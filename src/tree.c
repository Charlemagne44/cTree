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

char getCharacter(struct branch branch)
{
    int type = branch.type;
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
    int top, bottom, left, right = FALSE;
    if (y >= height - 1)
    {
        bottom = TRUE;
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
        // modify dy
        if (yroll <= 3 && !top)
        {
            returnDeltas.dy--;
        }
    }
    else if (life == middle)
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
        if (yroll <= 7 && !top)
        {
            returnDeltas.dy--;
        }
    }
    else if (life == old)
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
        if (yroll <= 1)
        {
            returnDeltas.dy--;
        }
    }

    // check for collisions
    int newy = branch.y + returnDeltas.dy;
    int newx = branch.x + returnDeltas.dx;
    if (checkCollision(win, newy, newx))
    {
        // if the new spot will have a collision, choose a random spot not occupied by neighbors
        // TODO - ENCOURAGE CURRENT TREND OF GROWTH
        int n = 0;
        struct deltas *neighborDelta = getNeighbors(win, branch.y, branch.x, &n);
        struct deltas *freeNeighbors = getFreeNeighbors(neighborDelta, n);
        int freeSize = 8 - n;
        int pickRoll = rollDie(0, freeSize - 1);
        returnDeltas.dy = freeNeighbors[pickRoll].dy;
        returnDeltas.dx = freeNeighbors[pickRoll].dx;
    }

    return returnDeltas;
}

int getNewType(struct deltas deltas)
{
    if (deltas.dy == 0)
    {
        if (deltas.dx == 0)
        {
            return trunk;
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
    else
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
    char newStr[2];
    newStr[0] = getCharacter(*newBranch);
    newStr[1] = '\0';
    newBranch->character = newStr;

    return newBranch;
}

void grow(WINDOW *win, struct branch *branch)
{
    // render current branch;
    mvwprintw(win, branch->y, branch->x, branch->character);
    wrefresh(win);
    getch();

    // if dead, run leaf probability and then return
    if (branch->life == dead)
    {
        if (branch->type != trunk)
        {
            // TODO - leaf logic
        }
        return;
    }

    // determine dy, and dx, and type;
    struct deltas deltas = getDelta(win, *branch);
    if (deltas.dx == 0 && deltas.dy == 0)
    {
        return;
    }
    int newType = getNewType(deltas);

    // grow the branches based upon age TODO - OTHER AGES
    int branchRoll = rollDie(1, 10);
    switch (branch->life)
    {
    case young:
        if (branchRoll <= 9) // 9/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
        }
    case middle:
        if (branchRoll <= 6) // 6/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
        }
    case old:
        if (branchRoll <= 3) // 3/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch);
        }
    case dead:
    }

    // increment age and decrement life
    branch->life++;

    // grow again if not dead
    grow(win, branch);
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

    char str[2];
    str[0] = getCharacter(*branch);
    str[1] = '\0';
    branch->character = str;

    // recursively grow the branch, and re-render the tree each time
    srand(time(0));
    grow(objects->treewin, branch);
}