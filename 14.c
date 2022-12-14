#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// Guessed
#define MAX 1000

#define _(x,y) ((y)*(MAX)+(x))

int part1(FILE *in);
int part2(FILE *in);
void fill(FILE *in, int *scan);
void draw(int *scan, int x1, int y1, int x2, int y2);
// Returns truthy if sand fell through
int dropSand(int *scan);

int Part = 1;
int MaxY = 0;

int main()
{
    FILE *in = fopen("in14", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    Part = 1;
    int scan[MAX*MAX] = {0};
    fill(in, scan);
    int i;
    for (i = 0; !dropSand(scan); i++);
    return i;
}

int part2(FILE *in)
{
    Part = 2;
    int scan[MAX*MAX] = {0};
    fill(in, scan);
    int i;
    for (i = 0; !dropSand(scan); i++);
    return i+1;
}

void fill(FILE *in, int *scan)
{
    int x = 0;
    int y = 0;
    int lastX = 0;
    int lastY = 0;
    MaxY = 0;
    int c;
    int isY = 0;
    while ((c = fgetc(in)) != EOF)
        switch(c)
        {
            case '-':
            case ' ':
                // Nothing to do
                break;
            case '>':
                if (fgetc(in) != ' ')
                    fprintf(stderr, "ERROR\n");
                if (lastX != 0)
                    draw(scan, lastX, lastY, x, y);
                if (y > MaxY)
                    MaxY = y;
                lastX = x;
                lastY = y;
                x = 0;
                y = 0;
                isY = 0;
                break;
            case '\n':
                if (lastX != 0)
                    draw(scan, lastX, lastY, x, y);
                lastX = 0;
                lastY = 0;
                x = 0;
                y = 0;
                isY = 0;
                break;
            case ',':
                isY++;
                break;
            default:
                if (c < '0' || c > '9')
                    fprintf(stderr, "ERROR\n");
                if (isY)
                    y = y*10 + c-'0';
                else
                    x = x*10 + c-'0';
                break;
        }
}

void draw(int *scan, int x1, int y1, int x2, int y2)
{
    if (x1 > x2 || y1 > y2)
    {
        int buff = x2;
        x2 = x1;
        x1 = buff;
        buff = y2;
        y2 = y1;
        y1 = buff;
    }

    for (int x = x1; x <= x2; x++)
        for (int y = y1; y <= y2; y++)
            scan[_(x,y)] = 1;
}

int dropSand(int *scan)
{
    int x = 500;
    int y = 0;
    while (y < MAX)
    {
        if (Part == 2 && y == MaxY + 1)
        {
            scan[_(x,y)] = 3;
            return 0;
        }
        if (!scan[_(x,y+1)])
            y++;
        else if (!scan[_(x-1,y+1)])
        {
            y++;
            x--;
        }
        else if (!scan[_(x+1,y+1)])
        {
            y++;
            x++;
        }
        else
        {
            scan[_(x,y)] = 2;
            if (x == 500 && y == 0)
                return 2;
            return 0;
        }
    }
    return 1;
}

