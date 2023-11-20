#include "tree.h"

int main(int argc, char **argv)
{
    struct ncursesObjects objects;
    init(&objects);
    refresh();
    __u_long seed = time(0);

    int c;
    while ((c = getopt(argc, argv, "hds:")) != -1)
    {
        switch (c)
        {
        case 'h':
            printHelp(&objects);
            cleanup(&objects);
            return 0;
        case 's':
            char *endptr;
            seed = strtoul(optarg, &endptr, 10);
        case 'd':
            printTimeSeed(&objects, seed);
        }
    }

    start(&objects, seed);

    move(0, 0);
    getch();
    cleanup(&objects);
    return 0;
}