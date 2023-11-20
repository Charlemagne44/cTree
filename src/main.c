#include "tree.h"

int main(int argc, char **argv)
{
    struct ncursesObjects objects;
    init(&objects);
    refresh();
    __u_long seed = time(0);

    int c;
    int live = FALSE;
    long sleepMilliseconds = 0;
    while ((c = getopt(argc, argv, "hds:l:")) != -1)
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
            break;
        case 'd':
            printTimeSeed(&objects, seed);
            break;
        case 'l':
            live = TRUE;
            sleepMilliseconds = strtol(optarg, &endptr, 10);
            break;
        }
    }

    start(&objects, seed, live, sleepMilliseconds);

    move(0, 0);
    getch();
    cleanup(&objects);
    return 0;
}