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
    return rand() & (upper - lower + 1);
}

struct deltas getDelta(struct branch branch)
{
    int height, width, y, x, life, type;
    getmaxyx(stdscr, height, width);
    y = branch.y;
    x = branch.y;
    life = branch.life;
    type = branch.type;
    struct deltas returnDeltas = {0, 0};
    int xroll, yroll;
    xroll = rollDie(1, 10);
    yroll = rollDie(1, 10);

    switch (type)
    {
    case trunk:
        if (life == young)
        {
            // modify dx
            if (xroll <= 5)
            {
                returnDeltas.dx++;
            }
            else if (xroll <= 10)
            {
                returnDeltas.dx--;
            }
            // modify dy
            if (yroll <= 3) // 3/10 chance height gets raised
            {
                returnDeltas.dy++;
            }
        }
        else if (life == middle)
        {
        }
        else if (life == old)
        {
        }
    // TODO - other cases
    case right:
        if (life == young)
        {
            if (xroll <= 6)
            {
                returnDeltas.dx++;
            }
            else if (xroll <= 8)
            {
                returnDeltas.dx--;
            }
        }
        else if (life == middle)
        {
        }
        else if (life == old)
        {
        }
    case left:
        if (life == young)
        {
            if (xroll <= 6)
            {
                returnDeltas.dx--;
            }
            else if (xroll <= 8)
            {
                returnDeltas.dx++;
            }
        }
        else if (life == middle)
        {
        }
        else if (life == old)
        {
        }
    }

    // rule out deltas that would bring the next branch off screen
    if (x <= 1 && returnDeltas.dx == -1)
    {
        returnDeltas.dx = 0;
    }
    else if (y <= 1 && returnDeltas.dy == -1)
    {
        returnDeltas.dy = 0;
    }
    else if (x >= width - 1 && returnDeltas.dx == 1)
    {
        returnDeltas.dx = 0;
    }
    else if (y >= height - 1 && returnDeltas.dy == 1)
    {
        returnDeltas.dy = 0;
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

void grow(WINDOW *win, struct branch *branch)
{
    // render current branch;
    mvwprintw(win, branch->y, branch->x, branch->character);
    wrefresh(win);
    // getch();

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
    struct deltas deltas = getDelta(*branch);
    int newType = getNewType(deltas);

    // recursively branch
    // int height, width;
    // getmaxyx(stdscr, height, width);
    int branchRoll = rollDie(1, 10);
    // int ageMultiplier = (dead - (branch->type)); // 3 is the max multipler (young), 1 is the lowest (old)
    // int heightPercentage = (1.0 - ((float)branch->y / (float)height));
    switch (branch->life)
    {
    case young:
        if (branchRoll < 9) // 9/10 chance to grow a young branch
        {
            struct branch *newBranch = malloc(sizeof(struct branch));
            newBranch->life = young;
            newBranch->type = newType;
            newBranch->x = branch->x + deltas.dx;
            newBranch->y = branch->y + deltas.dy;
            char newStr[2];
            newStr[0] = getCharacter(*newBranch);
            newStr[1] = '\0';
            newBranch->character = newStr;

            grow(win, newBranch);
        }
    case middle:
    case old:
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

int main()
{
    struct ncursesObjects objects;
    init(&objects);
    refresh();

    start(&objects);

    getch();
    cleanup(&objects);
    return 0;
}