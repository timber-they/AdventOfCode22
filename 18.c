#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// Actually MAX is the max value +1 and offsetting by another 1
#define MAX 24
//#define COUNT 13
#define COUNT 2834

#define _(x,y,z) ((x)+(y)*(MAX)+(z)*(MAX)*(MAX))

int part1(FILE *in);
int part2(FILE *in);
void fillRoom(FILE *in, int *map);
int countFreeSides(int *map);
void simulateSteam(int *map);
void fillWallsWithSteam(int *map);
void fillPockets(int *map);

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
    int map[MAX*MAX*MAX] = {0};
    fillRoom(in, map);
    simulateSteam(map);
    fillPockets(map);
    return countFreeSides(map);
}

void fillRoom(FILE *in, int *map)
{
    for (int i = 0; i < COUNT; i++)
    {
        int x, y, z;
        if (fscanf(in, "%d,%d,%d\n", &x, &y, &z) != 3)
            fprintf(stderr, "Read failed (i=%d)!\n", i);
        x++;
        y++;
        z++;
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

void simulateSteam(int *map)
{
    fillWallsWithSteam(map);
    int foundSteam = 1;
    while (foundSteam)
    {
        foundSteam = 0;
        for (int x = 0; x < MAX; x++)
            for (int y = 0; y < MAX; y++)
                for (int z = 0; z < MAX; z++)
                {
                    if (map[_(x,y,z)] != -1)
                        continue;
                    foundSteam = 1;
                    if (x != 0)
                    {
                        if (!map[_(x-1,y,z)])
                            map[_(x-1,y,z)] = -1;
                        else if (map[_(x-1,y,z)] == 1)
                            map[_(x-1,y,z)] = 2;
                    }
                    if (x != MAX-1)
                    {
                        if (!map[_(x+1,y,z)])
                            map[_(x+1,y,z)] = -1;
                        else if (map[_(x+1,y,z)] == 1)
                            map[_(x+1,y,z)] = 2;
                    }
                    if (y != 0)
                    {
                        if (!map[_(x,y-1,z)])
                            map[_(x,y-1,z)] = -1;
                        else if (map[_(x,y-1,z)] == 1)
                            map[_(x,y-1,z)] = 2;
                    }
                    if (y != MAX-1)
                    {
                        if (!map[_(x,y+1,z)])
                            map[_(x,y+1,z)] = -1;
                        else if (map[_(x,y+1,z)] == 1)
                            map[_(x,y+1,z)] = 2;
                    }
                    if (z != 0)
                    {
                        if (!map[_(x,y,z-1)])
                            map[_(x,y,z-1)] = -1;
                        else if (map[_(x,y,z-1)] == 1)
                            map[_(x,y,z-1)] = 2;
                    }
                    if (z != MAX-1)
                    {
                        if (!map[_(x,y,z+1)])
                            map[_(x,y,z+1)] = -1;
                        else if (map[_(x,y,z+1)] == 1)
                            map[_(x,y,z+1)] = 2;
                    }
                    map[_(x,y,z)] = -2;
                }
    }
}

void fillWallsWithSteam(int *map)
{
    int x, y, z;
    for (x = 0; x == 0; x++)
        for (y = 0; y < MAX; y++)
            for (z = 0; z < MAX; z++)
                map[_(x,y,z)] = -1;
    for (x = MAX-1; x == MAX-1; x++)
        for (y = 0; y < MAX; y++)
            for (z = 0; z < MAX; z++)
                map[_(x,y,z)] = -1;

    for (x = 0; x < MAX; x++)
        for (y = 0; y == 0; y++)
            for (z = 0; z < MAX; z++)
                map[_(x,y,z)] = -1;
    for (x = 0; x < MAX; x++)
        for (y = MAX-1; y == MAX-1; y++)
            for (z = 0; z < MAX; z++)
                map[_(x,y,z)] = -1;

    for (x = 0; x < MAX; x++)
        for (y = 0; y < MAX; y++)
            for (z = 0; z == 0; z++)
                map[_(x,y,z)] = -1;
    for (x = 0; x < MAX; x++)
        for (y = 0; y < MAX; y++)
            for (z = MAX-1; z == MAX-1; z++)
                map[_(x,y,z)] = -1;
}

void fillPockets(int *map)
{
    for (int i = 0; i < MAX*MAX*MAX; i++)
        if (map[i] == -1)
            fprintf(stderr, "Steam in pocket filling detected!\n");
        else if (map[i] == -2)
            // Drying steamed arrays
            map[i] = 0;
        else if (map[i] == 0)
            // Filling pocket
            map[i] = 3;
}

