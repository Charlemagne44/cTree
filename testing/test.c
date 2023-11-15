/* Home baked unit test file for all helper functions */
#include "tree.h"

int testCheckCollision()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    mvaddch(1, 1, 'c');
    refresh();
    int collision = checkCollision(stdscr, 1, 1);
    if (!collision)
    {
        endwin();
        printf("There was not a collision where there should have been\n");
        return FALSE;
    }

    collision = checkCollision(stdscr, 2, 2);
    if (collision)
    {
        endwin();
        printf("There was a collision where there should not have been\n");
        return FALSE;
    }

    endwin();
    return TRUE;
}

int testGetNeighbors()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    mvaddch(1, 1, 'c');
    mvaddch(1, 2, 'c');
    mvaddch(1, 3, 'c');
    int n = 0;

    struct deltas *neighbors = getNeighbors(stdscr, 2, 2, &n);

    // check to see they are of the correct values (-1, (-1,0,1))
    if (neighbors[0].dy != -1 || neighbors[0].dx != -1)
    {
        endwin();
        printf("delta 0 deltas were incorrect\n");
        return FALSE;
    }

    if (neighbors[1].dy != -1 || neighbors[1].dx != 0)
    {
        endwin();
        printf("delta 1 deltas were incorrect\n");
        return FALSE;
    }

    if (neighbors[2].dy != -1 || neighbors[2].dx != 1)
    {
        endwin();
        printf("delta 2 deltas were incorrect\n");
        return FALSE;
    }

    endwin();
    return TRUE;
}

int testGetFreeNeighbors()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    mvaddch(1, 1, 'c');
    mvaddch(1, 2, 'c');
    mvaddch(1, 3, 'c');
    int n = 0;
    refresh();

    struct deltas *neighbors = getNeighbors(stdscr, 2, 2, &n);
    struct deltas *freeSpots = getFreeNeighbors(neighbors, n);
    endwin();

    // MANUAL CHECK FOR DELTAS
    printf("n value %d\n", n);
    for (int i = 0; i < (8 - n); i++)
    {
        printf("free spot dy, dx: %d, %d\n", freeSpots[i].dy, freeSpots[i].dx);
    }
    return TRUE;
}

int testGetDelta()
{
    // check deltas for bottom edge
    for (int i = 0; i < 100; i++)
    {
        initscr();
        int height, width;
        getmaxyx(stdscr, height, width);
        struct branch branch = {
            trunk,
            young,
            width / 2,
            height - 1,
            "~",
        };
        struct deltas deltas = getDelta(stdscr, branch);
        if (deltas.dy > 0)
        {
            endwin();
            printf("bad delta (dy, dx): (%d, %d) on iteration: %d\n", deltas.dy, deltas.dx, i);
            return FALSE;
        }

        endwin();
    }

    // check deltas for top edge
    for (int i = 0; i < 100; i++)
    {
        initscr();
        int width = getmaxx(stdscr);
        struct branch branch = {
            trunk,
            young,
            width / 2,
            1,
            "~",
        };
        struct deltas deltas = getDelta(stdscr, branch);
        if (deltas.dy < 0)
        {
            endwin();
            printf("bad delta (dy, dx): (%d, %d) on iteration: %d\n", deltas.dy, deltas.dx, i);
            return FALSE;
        }

        endwin();
    }

    // check deltas for left edge
    for (int i = 0; i < 100; i++)
    {
        initscr();
        int height = getmaxy(stdscr);
        struct branch branch = {
            trunk,
            young,
            1,
            height / 2,
            "~",
        };
        struct deltas deltas = getDelta(stdscr, branch);
        if (deltas.dx < 0)
        {
            endwin();
            printf("bad delta (dy, dx): (%d, %d) on iteration: %d\n", deltas.dy, deltas.dx, i);
            return FALSE;
        }

        endwin();
    }

    // check deltas for right edge
    for (int i = 0; i < 100; i++)
    {
        initscr();
        int height, width;
        getmaxyx(stdscr, height, width);
        struct branch branch = {
            trunk,
            young,
            width - 1,
            height / 2,
            "~",
        };
        struct deltas deltas = getDelta(stdscr, branch);
        if (deltas.dx > 0)
        {
            endwin();
            printf("bad delta (dy, dx): (%d, %d) on iteration: %d\n", deltas.dy, deltas.dx, i);
            return FALSE;
        }

        endwin();
    }

    return TRUE;
}

int testRollDie()
{
    // run large iteration of die rolls to make sure they do not leave their bounds
    int lower = 1;
    int upper = 5;
    for (int i = 0; i < 200; i++)
    {
        int die = rollDie(lower, upper);
        if (die < lower || die > upper)
        {
            printf("die left range with roll %d\n", die);
            return FALSE;
        }
    }
    return TRUE;
}

int testGetCharacter()
{
    initscr();
    int height, width;
    getmaxyx(stdscr, height, width);
    endwin();

    // up left and down right
    char str[2];
    str[0] = getCharacter(upLeft);
    str[1] = '\0';

    if (strcmp(str, "\\") != 0)
    {
        printf("Wrong character: %s\n", str);
        return FALSE;
    }

    // up right and down left
    str[0] = getCharacter(upRight);
    str[1] = '\0';

    if (strcmp(str, "/") != 0)
    {
        printf("Wrong character: %s\n", str);
        return FALSE;
    }

    // up and down
    str[0] = getCharacter(up);
    str[1] = '\0';

    if (strcmp(str, "|") != 0)
    {
        printf("Wrong character: %s\n", str);
        return FALSE;
    }

    // left and rights
    str[0] = getCharacter(left);
    str[1] = '\0';

    if (strcmp(str, "-") != 0)
    {
        printf("Wrong character: %s\n", str);
        return FALSE;
    }

    return TRUE;
}

/* Run all the tests without assertions that reqiure looking at the output */
void runManualTests()
{
    if (!testGetFreeNeighbors())
        printf("testGetFreeNighbors failed\n");
}

int main()
{
    // seed for each run of testing for randomization
    srand(time(0));

    if (!testCheckCollision())
        printf("testCheckCollision Failed\n");

    if (!testGetNeighbors())
        printf("testGetNeighbors failed\n");

    if (!testGetDelta())
        printf("testGetDelta failed\n");

    if (!testRollDie())
        printf("testRollDie failed\n");

    if (!testGetCharacter())
        printf("testGetCharacter failed\n");

    // comment out too avoid manual tests
    // runManualTests();

    return 0;
}
