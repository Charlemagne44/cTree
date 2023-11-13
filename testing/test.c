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

int main()
{
    if (!testCheckCollision())
        printf("testCheckCollision Failed\n");

    if (!testGetNeighbors())
        printf("testGetNeighbors failed\n");

    if (!testGetFreeNeighbors())
        printf("testGetFreeNeighbors failed\n");
    return 0;
}
