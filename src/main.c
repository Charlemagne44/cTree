#include "tree.h"

int main(int argc, char **argv)
{
    struct ncursesObjects objects;
    init(&objects);
    refresh();

    int c;
    while ((c = getopt(argc, argv, "h")) != -1)
    {
        switch (c)
        {
        case 'h':
            printHelp(&objects);
        }
    }

    start(&objects);

    move(0, 0);
    getch();
    cleanup(&objects);
    return 0;
}