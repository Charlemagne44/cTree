#include <stdio.h>
#include <stdlib.h>

struct cat
{
    int paws;
    char *name;
};

void displayCatInfo(struct cat *cats, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("Name: %s, paws: %d\n", cats[i].name, cats[i].paws);
    }
}

void reallocCat(struct cat **cats, int new_size)
{
    *cats = realloc(*cats, new_size * sizeof(struct cat));
}

void newCat(struct cat *cats, int n)
{
    printf("n is: %d\n", n);
    cats[n - 1].name = "Funcy cat";
    cats[n - 1].paws = 10;
}

int main()
{
    int n = 3;
    struct cat *cat = malloc(n * sizeof(struct cat));
    char **names = malloc(n * sizeof(char *));
    names[0] = "James";
    names[1] = "Tom";
    names[2] = "Crysanthemum";
    for (int i = 0; i < n; i++)
    {
        cat[i].paws = i + 1;
        cat[i].name = names[i];
    }

    displayCatInfo(cat, n);

    printf("\nPrinting updated list!\n");
    n++;
    // cat = realloc(cat, (n * sizeof(struct cat)));
    reallocCat(&cat, n);
    cat[3].name = "Test";
    cat[3].paws = 4;

    displayCatInfo(cat, n);

    printf("\nThe NEWEST list: \n");
    n++;
    reallocCat(&cat, n);
    newCat(cat, n);
    displayCatInfo(cat, n);

    free(cat);
    free(names);
    return 0;
}