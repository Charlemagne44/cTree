#include "tree.h"

int main(int argc, char **argv)
{
    struct ncursesObjects objects;
    init(&objects);
    refresh();
    __u_long seed = time(0);

    int c;
    int live = FALSE;
    int infinite = FALSE;
    int debug = FALSE;
    long sleepMilliseconds = 0; // wait time between writing each character
    long waitMilliseconds = 0;  // wait time between each tree generated
    while ((c = getopt(argc, argv, "hds:l:iw:")) != -1)
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
            debug = TRUE;
            break;
        case 'l':
            live = TRUE;
            sleepMilliseconds = strtol(optarg, &endptr, 10);
            break;
        case 'i':
            infinite = TRUE;
            break;
        case 'w':
            waitMilliseconds = strtol(optarg, &endptr, 10);
            break;
        }
    }

    do
    {
        if (debug)
        {
            printTimeSeed(&objects, seed);
        }
        start(&objects, seed, live, sleepMilliseconds);
        if (infinite)
        {
            if (waitMilliseconds == 0)
                napms(INFINITE_WAIT_MILLSECONDS_BREAK);
            else
                napms(waitMilliseconds);
            werase(objects.treewin);
            wrefresh(objects.treewin);
        }
        seed = time(0);
        srand(seed);
    } while (infinite);

    move(0, 0);
    getch();
    cleanup(&objects);
    return 0;
}