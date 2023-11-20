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
    objects->helpwin = newwin(8, width / 2, 0, 0);
    objects->treewin = newwin(height, width, 0, 0);
    objects->debugwin = newwin(8, width / 3, 0, 0);

    objects->helpPanel = new_panel(objects->helpwin);
    objects->treePanel = new_panel(objects->treewin);
    objects->debugPanel = new_panel(objects->debugwin);

    set_panel_userptr(objects->treePanel, objects->helpPanel);
    set_panel_userptr(objects->helpPanel, objects->treePanel);
    set_panel_userptr(objects->treePanel, objects->debugPanel);
    set_panel_userptr(objects->debugPanel, objects->treePanel);

    update_panels();
}

/* Ncurses objects cleanup */
void cleanup(struct ncursesObjects *objects)
{
    delwin(objects->treewin);
    delwin(objects->basewin);
    delwin(objects->helpwin);
    endwin();
}

/* Make boxes around the ncurses objects for visualization */
void makeBoxes(struct ncursesObjects *objects)
{
    box(objects->treewin, 0, 0);
    wrefresh(objects->treewin);
    doupdate();
}

/* Print the time seed to assist in debugging from time based randomization */
void printTimeSeed(struct ncursesObjects *objects, time_t seed)
{
    mvwprintw(objects->debugwin, 1, 1, "Time seed: %ld\n", seed);
    box(objects->debugwin, 0, 0);
    wrefresh(objects->debugwin);
    top_panel(objects->debugPanel);
    update_panels();
    doupdate();
}

void printHelp(struct ncursesObjects *objects)
{
    int width = getmaxx(objects->helpwin);
    char *title = "C TREE GENERATOR";
    int titleLen = strlen(title);

    box(objects->helpwin, 0, 0);
    wmove(objects->helpwin, 1, width / 2 - (titleLen / 2));
    wattron(objects->helpwin, A_BOLD);
    wprintw(objects->helpwin, title);
    wattroff(objects->helpwin, A_BOLD);
    mvwprintw(objects->helpwin, 2, 1, "-h: Show help menu");
    mvwprintw(objects->helpwin, 3, 1, "-d: Show seed debug information");
    mvwprintw(objects->helpwin, 4, 1, "-s: Supply a seed to the randomizer");
    mvwprintw(objects->helpwin, 5, 1, "-l: Watch the growth slowed by your milliseconds arg");
    mvwprintw(objects->helpwin, 6, 1, "-i: Infinitely generate trees");

    wrefresh(objects->treewin);
    top_panel(objects->helpPanel);
    update_panels();
    doupdate();

    getch();
    top_panel(objects->treePanel);
    update_panels();
    doupdate();
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
    if (checkCollision(win, newy, newx))
    {
        // if the new spot will have a collision, have a chance to choose a random spot of free neighbors
        // else -> return 0, 0 to discourage more branching
        // older branches higher off the ground should have a higher probability of searching for free neighbors
        float heightMultiplier = 1.0 + (1.0 - ((float)branch.y / (float)height));
        float ageMultipier = 1.0 + (1.0 / (1 + branch.life));
        if ((float)rollDie(1, 10) * ageMultipier * heightMultiplier <= 5.0)
        {
            int n = 0;
            struct deltas *neighborDelta = getNeighbors(win, branch.y, branch.x, &n);
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
    newBranch->character = getString(type);

    return newBranch;
}

/* Bud leaves on eligible surrounding tiles after a branch has died within the grow function */
void bud(WINDOW *win, int y, int x, int live, long sleep)
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
                doupdate();
                if (live)
                    napms(sleep);
            }
        }
    }
}

/* Main recursive growth functino kicked off from start */
void grow(WINDOW *win, struct branch *branch, int live, long sleep)
{
    // either wait for input or sleep depending on config
    if (KEY_BETWEEN_RENDER)
        getch();
    else if (live)
    {
        napms(sleep);
    }

    // render current branch and account for special 2 character branches that need to be printed with a shift
    if (branch->type == trunkLeft || branch->type == leftUp || branch->type == leftDown)
    {
        mvwprintw(win, branch->y, branch->x - 1, branch->character);
        wrefresh(win);
        doupdate();
    }
    else if (branch->type == trunkRight || branch->type == rightUp || branch->type == rightDown)
    {
        mvwprintw(win, branch->y, branch->x, branch->character);
        branch->x = branch->x + 1;
        wrefresh(win);
        doupdate();
    }
    else
    {
        mvwprintw(win, branch->y, branch->x, branch->character);
        wrefresh(win);
        doupdate();
    }

    // determine dy, and dx, and type;
    struct deltas deltas = getDelta(win, *branch);
    if (deltas.dx == 0 && deltas.dy == 0)
    {
        branch->life = dead; // boxed in
    }

    int newType = getNewType(deltas, branch->type);

    // set height related growth parameters before budding and growing
    int height = getmaxy(win);
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
                grow(win, newBranch, live, sleep);
                return;
            }
            else
            {
                branch->character = "&";
                bud(win, branch->y, branch->x, live, sleep);
                mvwprintw(win, branch->y, branch->x, branch->character);
                wrefresh(win);
                doupdate();
                return;
            }
        }

        if (branch->type != trunk && heightPercentage > LEAF_HEIGHT_PERCENTAGE_MIN)
        {
            branch->character = "&";
            bud(win, branch->y, branch->x, live, sleep);
            mvwprintw(win, branch->y, branch->x, branch->character);
            wrefresh(win);
            doupdate();
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
            grow(win, newBranch, live, sleep);
        }
        else if (branchRoll <= 9 || heightPercentage <= LEAF_HEIGHT_PERCENTAGE_MIN) // 9/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
        }
        break;
    case middle:
        if (branchRoll <= 6) // 6/10 chance to grow a middle aged branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
        }
        break;
    case old:
        if (branchRoll <= 3) // 3/10 chance to grow an old branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
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
        grow(win, branch, live, sleep);
    }
}

/* Initiailze necessary parameters to begin growth, and then start the recursion */
void start(struct ncursesObjects *objects, __u_long seed, int live, long sleep)
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
    branch->character = getString(branch->type);

    // recursively grow the branch, and re-render the tree each time
    srand((int)seed);
    grow(objects->treewin, branch, live, sleep);
}