#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define WIDTH 150
#define HEIGHT 20
#define MAX_TIME ((WIDTH)*(HEIGHT))

#define _(x,y,time) ((time)*(WIDTH)*(HEIGHT) + (y)*(WIDTH) + (x))

typedef struct Blizzard
{
    int x;
    int y;
    int direction;
} Blizzard;

int part1(FILE *in);
int part2(FILE *in);
int read(FILE *in, Blizzard *buff);
void blockedDirections(Blizzard *blizzards, int count, int x, int y, int *blocked);
void iterate(Blizzard *blizzards, int count);
void unIterate(Blizzard *blizzards, int count);
int shortestPath(Blizzard *blizzards, int count, int x, int y, int *memory, int time, int currentShortest);

int main()
{
    FILE *in = fopen("in24", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    Blizzard blizzards[WIDTH*HEIGHT] = {0};
    int count = read(in, blizzards);
    int *memory = calloc(WIDTH*HEIGHT*MAX_TIME, sizeof(*memory));
    return shortestPath(blizzards, count, 0, -1, memory, 0, 1<<30);
}

int part2(FILE *in)
{
    return in == NULL ? -3 : -2;
}

int read(FILE *in, Blizzard *buff)
{
    int index = 0;
    for (int i = 0; i < WIDTH+2; i++)
        if (fgetc(in) == '\n')
            fprintf(stderr, "Didn't expect that linbreak\n");
    if (fgetc(in) != '\n')
        fprintf(stderr, "Didn't expect that missing linebreak\n");
    for (int y = 0; y < HEIGHT; y++)
    {
        if (fgetc(in) != '#')
            fprintf(stderr, "Didn't expect that missing wall\n");
        for (int x = 0; x < WIDTH; x++)
        {
            switch(fgetc(in))
            {
                case '.':
                    break;
                case '>':
                    buff[index++] = (Blizzard)
                    {
                        .x = x,
                        .y = y,
                        .direction = 0
                    };
                    break;
                case 'v':
                    buff[index++] = (Blizzard)
                    {
                        .x = x,
                        .y = y,
                        .direction = 1
                    };
                    break;
                case '<':
                    buff[index++] = (Blizzard)
                    {
                        .x = x,
                        .y = y,
                        .direction = 2
                    };
                    break;
                case '^':
                    buff[index++] = (Blizzard)
                    {
                        .x = x,
                        .y = y,
                        .direction = 3
                    };
                    break;
            }
        }
        if (fgetc(in) != '#')
            fprintf(stderr, "Didn't expect that missing wall\n");
        if (fgetc(in) != '\n')
            fprintf(stderr, "Didn't expect that missing line break\n");
    }
    return index;
}

void blockedDirections(Blizzard *blizzards, int count, int x, int y, int *blocked)
{
    blocked[0] = x >= WIDTH-1;
    blocked[1] = y >= HEIGHT-1;
    blocked[2] = x <= 0;
    blocked[3] = y <= 0;
    for (int i = 0; i < count; i++)
    {
        if (blizzards[i].x == x+1 && blizzards[i].y == y)
            blocked[0] = 1;
        else if (blizzards[i].x == x && blizzards[i].y == y+1)
            blocked[1] = 1;
        else if (blizzards[i].x == x-1 && blizzards[i].y == y)
            blocked[2] = 1;
        else if (blizzards[i].x == x && blizzards[i].y == y-1)
            blocked[3] = 1;
        else if (blizzards[i].x == x && blizzards[i].y == y)
            blocked[4] = 1;
    }
}

void iterate(Blizzard *blizzards, int count)
{
    for (int i = 0; i < count; i++)
    {
        switch(blizzards[i].direction)
        {
            case 0:
                blizzards[i].x++;
                break;
            case 1:
                blizzards[i].y++;
                break;
            case 2:
                blizzards[i].x--;
                break;
            case 3:
                blizzards[i].y--;
                break;
        }
        while (blizzards[i].x < 0)
            blizzards[i].x += WIDTH;
        while (blizzards[i].y < 0)
            blizzards[i].y += HEIGHT;
        blizzards[i].x = blizzards[i].x % WIDTH;
        blizzards[i].y = blizzards[i].y % HEIGHT;
    }
}

void unIterate(Blizzard *blizzards, int count)
{
    for (int i = 0; i < count; i++)
    {
        switch(blizzards[i].direction)
        {
            case 0:
                blizzards[i].x--;
                break;
            case 1:
                blizzards[i].y--;
                break;
            case 2:
                blizzards[i].x++;
                break;
            case 3:
                blizzards[i].y++;
                break;
        }
        while (blizzards[i].x < 0)
            blizzards[i].x += WIDTH;
        while (blizzards[i].y < 0)
            blizzards[i].y += HEIGHT;
        blizzards[i].x = blizzards[i].x % WIDTH;
        blizzards[i].y = blizzards[i].y % HEIGHT;
    }
}

int shortestPath(Blizzard *blizzards, int count, int x, int y, int *memory, int time, int currentShortest)
{
    if (time >= MAX_TIME)
    {
        //fprintf(stderr, "Exceeded time limit!\n");
        return 1<<30;
    }
    if (y >= 0)
    {
        if (memory[_(x,y,time)])
            return memory[_(x,y,time)];
        if (x == WIDTH-1 && y == WIDTH-1)
            return (memory[_(x,y,time)] = 1);
        if (currentShortest <= 0)
            return 1<<30;
    }

    int blocked[5] = {0};
    blockedDirections(blizzards, count, x, y, blocked);
    iterate(blizzards, count);
    int min = currentShortest;

    if (!blocked[0])
    {
        int new = shortestPath(blizzards, count, x+1, y, memory, time+1, min-1) + 1;
        if (new < min)
            min = new;
    }
    if (!blocked[1])
    {
        int new = shortestPath(blizzards, count, x, y+1, memory, time+1, min-1) + 1;
        if (new < min)
            min = new;
    }
    if (!blocked[2])
    {
        int new = shortestPath(blizzards, count, x-1, y, memory, time+1, min-1) + 1;
        if (new < min)
            min = new;
    }
    if (!blocked[3])
    {
        int new = shortestPath(blizzards, count, x, y-1, memory, time+1, min-1) + 1;
        if (new < min)
            min = new;
    }
    if (!blocked[4])
    {
        // Do nothing
        int new = shortestPath(blizzards, count, x, y, memory, time+1, min-1) + 1;
        if (new < min)
            min = new;
    }
    unIterate(blizzards, count);
    if (min < currentShortest)
    {
        return y >= 0
            ? (memory[_(x,y,time)] = min)
            : min;
    }
    else
        return 1<<30;
}

