#include "tree.h"

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