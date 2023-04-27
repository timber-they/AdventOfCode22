#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// Actually MAX is the max value +1
#define MAX 22
//#define COUNT 13
#define COUNT 2834

#define _(x,y,z) ((x)+(y)*(MAX)+(z)*(MAX)*(MAX))

int part1(FILE *in);
int part2(FILE *in);
void fillRoom(FILE *in, int *map);
int countFreeSides(int *map);

int main()
{
    FILE *in = fopen("in18", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int map[MAX*MAX*MAX] = {0};
    fillRoom(in, map);
    return countFreeSides(map);
}

int part2(FILE *in)
{
    return in == NULL ? -3 : -2;
}

void fillRoom(FILE *in, int *map)
{
    for (int i = 0; i < COUNT; i++)
    {
        int x, y, z;
        if (fscanf(in, "%d,%d,%d\n", &x, &y, &z) != 3)
            fprintf(stderr, "Read failed (i=%d)!\n", i);
        if (x >= MAX || y >= MAX || z >= MAX)
            fprintf(stderr, "Value too large (i=%d)!\n", i);
        map[_(x,y,z)] = 1;
    }
}

int countFreeSides(int *map)
{
    int count = 0;
    for (int x = 0; x < MAX; x++)
        for (int y = 0; y < MAX; y++)
            for (int z = 0; z < MAX; z++)
            {
                if (!map[_(x,y,z)])
                    continue;
                count += (x == 0 || !map[_(x-1,y,z)]);
                count += (x == MAX-1 || !map[_(x+1,y,z)]);
                count += (y == 0 || !map[_(x,y-1,z)]);
                count += (y == MAX-1 || !map[_(x,y+1,z)]);
                count += (z == 0 || !map[_(x,y,z-1)]);
                count += (z == MAX-1 || !map[_(x,y,z+1)]);
            }
    return count;
}

