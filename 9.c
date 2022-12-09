#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MAX_KNOWN 100000

int part1(FILE *in);
int part2(FILE *in);
void move(char dir, int *hx, int *hy);
void adjustTail(int hx, int hy, int *tx, int *ty);
int enterPosition(int *known, int count, int x, int y);
int abs(int x);
int sgn(int x);

int main()
{
    FILE *in = fopen("in9", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    char dir;
    int count;
    int known[MAX_KNOWN];
    known[0] = 0;
    known[1] = 0;
    int knownCount = 2;
    int hx = 0,
        hy = 0,
        tx = 0,
        ty = 0;
    while(fscanf(in, "%c %d\n", &dir, &count) == 2)
        for (int i = 0; i < count; i++)
        {
            move(dir, &hx, &hy);
            adjustTail(hx, hy, &tx, &ty);
            knownCount = enterPosition(known, knownCount, tx, ty);
        }
    return knownCount / 2;
}

int part2(FILE *in)
{
    char dir;
    int count;
    int known[MAX_KNOWN];
    known[0] = 0;
    known[1] = 0;
    int knownCount = 2;
    int positions[20] = {0};
    while(fscanf(in, "%c %d\n", &dir, &count) == 2)
        for (int i = 0; i < count; i++)
        {
            move(dir, positions, positions+1);
            for (int i = 2; i < 19; i+=2)
                adjustTail(positions[i-2], positions[i-1], positions+i, positions+i+1);
            knownCount = enterPosition(known, knownCount, positions[18], positions[19]);
        }
    return knownCount / 2;
}

void move(char dir, int *hx, int *hy)
{
    switch(dir)
    {
        case 'R':
            (*hx)++;
            break;
        case 'D':
            (*hy)++;
            break;
        case 'L':
            (*hx)--;
            break;
        case 'U':
            (*hy)--;
            break;
    }
}

void adjustTail(int hx, int hy, int *tx, int *ty)
{
    if (abs(hx-*tx) <= 1 && abs(hy-*ty) <= 1)
        return;
    *tx += sgn(hx-*tx);
    *ty += sgn(hy-*ty);
    if (abs(hx-*tx) > 1 || abs(hy-*ty) > 1)
    {
        fprintf(stderr, "Lost tail!\n");
        exit(1);
    }
}

int abs(int x)
{
    return x < 0 ? -x : x;
}

int sgn(int x)
{
    return x < 0 ? -1 : x > 0 ? 1 : 0;
}

int enterPosition(int *known, int count, int x, int y)
{
    for (int i = 0; i < count-1; i+=2)
        if (known[i] == x && known[i+1] == y)
            return count;
    known[count++] = x;
    known[count++] = y;
    return count;
}

