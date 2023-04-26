#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define ITERATIONS 2022
#define SIZE 4
#define WIDTH 7
#define JETS 10091
#define PATTERNS 5
#define OFFSET_X 2
#define OFFSET_Y (-4)

#define _(x,y) ((x)+(WIDTH)*(y))
#define BMG(i,x,y) ((patterns)[(x)+(y)*(SIZE)+(i)*(SIZE)*(SIZE)])

int *patterns =
{
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    ///////////
    0, 1, 0, 0,
    1, 1, 1, 0,
    0, 1, 0, 0,
    0, 0, 0, 0,
    ///////////
    0, 0, 1, 0,
    0, 0, 1, 0,
    1, 1, 1, 0,
    0, 0, 0, 0,
    ///////////
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 0, 0, 0,
    ///////////
    1, 1, 0, 0,
    1, 1, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

int part1(FILE *in);
int part2(FILE *in);
int collides(int part, int *map, int x, int y);
void draw(int part, int *map, int x, int y);
void readJets(FILE *in, int *jets);
int drop(int part, int *map, int height);

int main()
{
    FILE *in = fopen("in17", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int jets[JETS];
    readJets(in, jets);
}

int part2(FILE *in)
{
    return -2;
}

int collides(int part, int *map, int x, int y)
{
    for (int xi = 0; xi < SIZE; xi++)
        for (int yi = 0; yi < SIZE; yi++)
            if (BMG(part, xi, yi) && map[_(x+xi,y+yi)])
                return 1;
    return 0;
}

void draw(int part, int *map, int x, int y)
{
    for (int xi = 0; xi < SIZE; xi++)
        for (int yi = 0; yi < SIZE; yi++)
            if (BMG(part, xi, yi))
                map[_(x+xi,y+yi)] = 1;
}

void readJets(FILE *in, int *jets)
{
    for (int i = 0; i < JETS; i++)
        jets[i] = fgetc(in) == '>' ? 1 : -1;
}

int drop(int part, int *map, int height)
{
    int x = OFFSET_X;
    int y = ITERATIONS*SIZE-height-OFFSET_Y;
}

