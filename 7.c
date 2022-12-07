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
	int completeSize;
} Tree;

int part1(FILE *in);
int part2(FILE *in);
Tree *parseTree(FILE *in);
void freeTree(Tree tree);
void printTree(Tree tree, int depth);
int getSize(Tree *tree);
int sumWithMaximum(Tree *tree, int max);
int containsDirectoryName(Tree tree, char *name);
int findSmallestDirectoryOfAtLeast(Tree *tree, int size);

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
	int res = sumWithMaximum(parsed, 100000);
    freeTree(*parsed);
    free(parsed);
	return res;
}

int part2(FILE *in)
{
    Tree *parsed = parseTree(in);
	int res = findSmallestDirectoryOfAtLeast(parsed, 30000000 - 70000000 + getSize(parsed));
    freeTree(*parsed);
    free(parsed);
	return res;
}

Tree *parseTree(FILE *in)
{
    Tree root =
    {
        .name = "",
        .parent = NULL,
        .children = malloc(MAX_CHILDREN * sizeof(Tree)),
        .childrenCount = 0,
        .directSize = 0,
		.completeSize = -1
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
                            {
                                char *name = line + strlen("$ cd ");
                                int known;
                                if ((known = containsDirectoryName(*current, name)))
                                {
                                    current = current->children+known-1;
                                    break;
                                }
                                current->children[current->childrenCount++] =
                                    (Tree) {
                                        .name = "",
                                        .parent = current,
                                        .children = malloc(MAX_CHILDREN * sizeof(Tree)),
                                        .childrenCount = 0,
                                        .directSize = 0,
										.completeSize = -1
                                    };
                                current = current->children + current->childrenCount - 1;
                                strncpy(current->name, name, MAX_NAME);
                                break;
                            }
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

void printTree(Tree tree, int depth)
{
	for (int i = 0; i < depth; i++)
		printf(" ");
	printf("%d %s", tree.completeSize, tree.name);
	for (int i = 0; i < tree.childrenCount; i++)
		printTree(tree.children[i], depth+1);
}

int getSize(Tree *tree)
{
	if (tree->completeSize >= 0)
		return tree->completeSize;
	tree->completeSize = tree->directSize;
	for (int i = 0; i < tree->childrenCount; i++)
		tree->completeSize += getSize(tree->children+i);
	return tree->completeSize;
}

int sumWithMaximum(Tree *tree, int max)
{
	int res = 0;
	if (getSize(tree) <= max)
		res += getSize(tree);
	for (int i = 0; i < tree->childrenCount; i++)
		res += sumWithMaximum(tree->children+i, max);
	return res;
}

int containsDirectoryName(Tree tree, char *name)
{
    for (int i = 0; i < tree.childrenCount; i++)
        if (!strcmp(tree.name, name))
            return i+1;
    return 0;
}

int findSmallestDirectoryOfAtLeast(Tree *tree, int size)
{
    if (getSize(tree) < size)
        return 0;
    int min = getSize(tree);
    for (int i = 0; i < tree->childrenCount; i++)
    {
        int new = findSmallestDirectoryOfAtLeast(tree->children+i, size);
        if (new && new < min)
            min = new;
    }
    return min;
}
