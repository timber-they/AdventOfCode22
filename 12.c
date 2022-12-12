#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define ROWS 41
#define COLUMNS 181

#define _(x,y) ((y)*(COLUMNS)+(x))

int part1(FILE *in);
int part2(FILE *in);
int *read(FILE *in, int *buff, int *sX, int *sY, int *eX, int *eY);
int abs(int x);
int fewestSteps(int *map, int sX, int sY, int eX, int eY);
int possible(int *map, int x1, int y1, int x2, int y2);

int part = 1;

int main()
{
    FILE *in = fopen("in12", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    part = 1;
    int map[ROWS*COLUMNS];
    int sX, sY, eX, eY;
    read(in, map, &sX, &sY, &eX, &eY);
    return fewestSteps(map, sX, sY, eX, eY);
}

int part2(FILE *in)
{
    part = 2;
    int map[ROWS*COLUMNS];
    int sX, sY, eX, eY;
    read(in, map, &sX, &sY, &eX, &eY);
    return fewestSteps(map, sX, sY, eX, eY);
}

int *read(FILE *in, int *buff, int *sX, int *sY, int *eX, int *eY)
{
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLUMNS; x++)
        {
            int c = fgetc(in);
            if (c == 'S')
            {
                buff[_(x,y)] = 1;
                *sX = x;
                *sY = y;
            }
            else if (c == 'E')
            {
                buff[_(x,y)] = 'z'-'a'+1;
                *eX = x;
                *eY = y;
            }
            else if (c <= 'z' && c >= 'a')
                buff[_(x,y)] = c-'a'+1;
            else
                fprintf(stderr, "ERROR\n");
        }

        if (fgetc(in) != '\n')
            fprintf(stderr, "ERROR\n");
    }
    return buff;
}

int abs(int x)
{
    return x < 0 ? -x : x;
}

int fewestSteps(int *map, int sX, int sY, int eX, int eY)
{
    if (sX == eX && sY == eY)
        return 0;
    int dist[ROWS*COLUMNS];
    for (int i = 0; i < ROWS*COLUMNS; i++)
        dist[i] = 1<<30;
    dist[_(eX,eY)] = 0;
    int minX = eX,
        minY = eY,
        maxX = eX,
        maxY = eY;
    int anythingNew = 1;
    while (anythingNew)
    {
        anythingNew = 0;
        for (int y = minY-1; y <= maxY+1; y++)
            for (int x = minX-1; x <= maxX+1; x++)
            {
                if (x < 0 || y < 0 || x >= COLUMNS || y >= ROWS)
                    continue;
                if (dist[_(x,y)] < 1<<30)
                    continue;
                int new;
                if (possible(map, x, y, x+1, y) &&
                    (new = dist[_(x+1,y)]+1) < dist[_(x,y)])
                    dist[_(x,y)] = new;
                if (possible(map, x, y, x-1, y) &&
                    (new = dist[_(x-1,y)]+1) < dist[_(x,y)])
                    dist[_(x,y)] = new;
                if (possible(map, x, y, x, y+1) &&
                    (new = dist[_(x,y+1)]+1) < dist[_(x,y)])
                    dist[_(x,y)] = new;
                if (possible(map, x, y, x, y-1) &&
                    (new = dist[_(x,y-1)]+1) < dist[_(x,y)])
                    dist[_(x,y)] = new;
                if (dist[_(x,y)] < 1<<30)
                {
                    if (x < minX)
                        minX = x;
                    if (x > maxX)
                        maxX = x;
                    if (y < minY)
                        minY = y;
                    if (y > maxY)
                        maxY = y;
                    anythingNew = 1;
                }
            }
    }
    if (part == 2)
    {
        int min = 1<<30;
        for (int y = 0; y < ROWS; y++)
            for (int x = 0; x < COLUMNS; x++)
                if (map[_(x,y)] == 1 && dist[_(x,y)] < min)
                    min = dist[_(x,y)];
        if (min < 1<<30)
            return min;
    }
    return dist[_(sX,sY)];
}

int possible(int *map, int x1, int y1, int x2, int y2)
{
    if (x1 < 0 || y1 < 0 || x1 >= COLUMNS || y1 >= ROWS ||
        x2 < 0 || y2 < 0 || x2 >= COLUMNS || y2 >= ROWS)
        return 0;
    int res = map[_(x2,y2)]-map[_(x1,y1)] <= 1;
    return res;
}

