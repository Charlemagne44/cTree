#include "web_tree.h"
#include <getopt.h>

int main(int argc, char **argv)
{
    int c;
    int live = false;
    int infinite = false;
    int height = 0;
    int width = 0;
    long sleepMilliseconds = 0; // wait time between writing each character
    char *endptr;
    while ((c = getopt(argc, argv, "l:ih:w:")) != -1)
    {
        switch (c)
        {
        case 'l':
            live = true;
            sleepMilliseconds = strtol(optarg, &endptr, 10);
            break;
        case 'i':
            infinite = true;
            break;
        case 'h':
            height = atoi(optarg);
            break;
        case 'w':
            width = atoi(optarg);
            break;
        }
    }

    __u_long seed = time(0);
    struct tree tree;
    init(&tree, height, width);

    do
    {
        start(&tree, seed, live, sleepMilliseconds, height, width);
        if (infinite)
        {
            sleep(INFINITE_WAIT_SECONDS_BREAK);
        }
        seed = time(0);
        srand(seed);
    } while (infinite);

    cleanup(&tree);
    return 0;
}