#include "web_tree.h"

void init(struct tree *tree, int height, int width)
{
    tree->entries = malloc((height * width) * sizeof(struct entry));
}

void grow(struct tree *tree, struct branch *branch, int live, long sleep_time, int height, int width)
{
    if (live)
    {
        sleep(sleep_time);
    }

    // render current branch and account for special 2 character branches that need to be printed with a shift
    if (branch->type == trunkLeft || branch->type == leftUp || branch->type == leftDown)
    {
        tree->entries->size = tree->entries->size + 1;
        tree->entries[tree->entries->size].character = branch->character;
        tree->entries[tree->entries->size].y = branch->y;
        tree->entries[tree->entries->size].x = branch->x;
    }
    else if (branch->type == trunkRight || branch->type == rightUp || branch->type == rightDown)
    {
        mvwprintw(win, branch->y, branch->x, branch->character);
        branch->x = branch->x + 1;
        wrefresh(win);
        doupdate();
    }
    else
    {
        mvwprintw(win, branch->y, branch->x, branch->character);
        wrefresh(win);
        doupdate();
    }

    // determine dy, and dx, and type;
    struct deltas deltas = getDelta(win, *branch);
    if (deltas.dx == 0 && deltas.dy == 0)
    {
        branch->life = dead; // boxed in
    }

    int newType = getNewType(deltas, branch->type);

    // set height related growth parameters before budding and growing
    int height = getmaxy(win);
    float heightPercentage = 1.0 - ((float)branch->y / (float)height);

    // if dead, run leaf budding and then return
    if (branch->life == dead)
    {
        // don't let a path end on a downward trend
        if (branch->type == down || branch->type == downLeft || branch->type == downRight)
        {
            // random chance to either bud or a last chance branch
            // all paths within must return
            if (rollDie(1, 10) <= 5)
            {
                struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
                grow(win, newBranch, live, sleep);
                free(newBranch);
                return;
            }
            else
            {
                branch->character = "&";
                bud(win, branch->y, branch->x, live, sleep);
                mvwprintw(win, branch->y, branch->x, branch->character);
                wrefresh(win);
                doupdate();
                return;
            }
        }

        if (branch->type != trunk && heightPercentage > LEAF_HEIGHT_PERCENTAGE_MIN)
        {
            branch->character = "&";
            bud(win, branch->y, branch->x, live, sleep);
            mvwprintw(win, branch->y, branch->x, branch->character);
            wrefresh(win);
            doupdate();
        }
        return;
    }

    int branchRoll = rollDie(1, 10);
    switch (branch->life)
    {
    case young:
        if (branch->type == trunk) // young trunk -> 100% growth chance
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
            free(newBranch);
        }
        else if (branchRoll <= 9 || heightPercentage <= LEAF_HEIGHT_PERCENTAGE_MIN) // 9/10 chance to grow a young branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
            free(newBranch);
        }
        break;
    case middle:
        if (branchRoll <= 6) // 6/10 chance to grow a middle aged branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
            free(newBranch);
        }
        break;
    case old:
        if (branchRoll <= 3) // 3/10 chance to grow an old branch
        {
            struct branch *newBranch = createNewBranch(young, newType, deltas, branch);
            grow(win, newBranch, live, sleep);
            free(newBranch);
        }
        break;
    case dead:
        break;
    }

    // increment age and decrement life
    branch->life++;

    // roll a chance to grow again if not dead
    if (rollDie(1, 10) <= 6 && branch->life != dead)
    {
        grow(win, branch, live, sleep);
    }
}

/* Initiailze necessary parameters to begin growth, and then start the recursion */
void start(struct tree *tree, __u_long seed, int live, long sleep, int height, int width)
{

    // initalize the array of branch objects
    int n = STARTING_TRUNKS;
    struct branch *branch = malloc(n * sizeof(struct branch));
    branch->life = young;
    branch->type = trunk;
    branch->y = height - 1;
    branch->x = (width / 2);
    branch->parentType = trunk;
    branch->character = getString(branch->type);

    // recursively grow the branch, and re-render the tree each time
    srand((int)seed);
    grow(tree, branch, live, sleep, height, width);
    free(branch);
}

void cleanup(struct tree *tree)
{
    free(tree->entries);
}