#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// Guessed
#define MAX_CHILDREN 10
#define MAX_NAME 10

typedef struct Tree
{
    char name[MAX_NAME+1];
    struct Tree *parent;
    struct Tree *children;
    int childrenCount;
    // The sum of the sizes of all the files that
    //  are directly in this directory
    int directSize;
} Tree;

int part1(FILE *in);
int part2(FILE *in);
Tree *parseTree(FILE *in);
void freeTree(Tree tree);
void printTree(Tree tree);

int main()
{
    FILE *in = fopen("in7", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    Tree *parsed = parseTree(in);
    freeTree(*parsed);
    free(parsed);
    return -1;
}

int part2(FILE *in)
{
    return in == NULL ? -3 : -2;
}

Tree *parseTree(FILE *in)
{
    Tree root =
    {
        .name = "",
        .parent = NULL,
        .children = malloc(MAX_CHILDREN * sizeof(Tree)),
        .childrenCount = 0,
        .directSize = 0
    };
    Tree *current = malloc(sizeof(*current));
    *current = root;
    Tree *res = current;
    char *line = NULL;
    size_t n = 0;

    while (getline(&line, &n, in) > 1)
    {
        switch (line[0])
        {
            case '$':
                // Command mode
                switch (line[2])
                {
                    case 'c':
                        // cd
                        switch (line[5])
                        {
                            case '/':
                                current = res;
                                break;
                            case '.':
                                current = current->parent;
                                break;
                            default:
                                current->children[current->childrenCount++] =
                                    (Tree) {
                                        .name = "",
                                        .parent = current,
                                        .children = malloc(MAX_CHILDREN * sizeof(Tree)),
                                        .childrenCount = 0,
                                        .directSize = 0
                                    };
                                current = current->children + current->childrenCount - 1;
                                strncpy(current->name, line + strlen("$ cd "), MAX_NAME);
                                break;
                        }
                        break;
                    case 'l':
                        // ls
                        // Can be ignored
                        break;
                }
                break;
            case 'd':
                // Directory - can be ignored
                break;
            default:
                // File - starts with size
                current->directSize += atoi(line);
                break;
        }
    }
    free(line);
    return res;
}

void freeTree(Tree tree)
{
    for (int i = 0; i < tree.childrenCount; i++)
        freeTree(tree.children[i]);
    free(tree.children);
}

