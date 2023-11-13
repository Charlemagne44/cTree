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

int main()
{
    if (!testCheckCollision())
        printf("testCheckCollision Failed\n");
    return 0;
}
