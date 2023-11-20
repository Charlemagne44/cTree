#include "tree.h"

int main()
{
    struct ncursesObjects objects;
    init(&objects);
    refresh();

    start(&objects);

    move(0, 0);
    getch();
    cleanup(&objects);
    return 0;
}