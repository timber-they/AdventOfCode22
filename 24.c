#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define WIDTH 150
#define HEIGHT 20
#define LOOPING 300
#define MAX_TIME ((WIDTH)*(HEIGHT))

#define _(x,y,time) (((time)%(LOOPING))*(WIDTH)*(HEIGHT) + (y)*(WIDTH) + (x))
#define abs(x) ((x) < 0 ? -(x) : (x))

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
void generateMaps(Blizzard *blizzards, int count, int *maps);
int bfs(int *maps, int startX, int startY, int startT, int endX, int endY);
int doesEdgeExist(int *maps, int sx, int sy, int st, int ex, int ey, int et);
void printMap(int *maps, int t);

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
    int maps[WIDTH*HEIGHT*LOOPING] = {0};
    generateMaps(blizzards, count, maps);
    for (int t = 1; t <= LOOPING; t++)
    {
        int time = bfs(maps, 0, 0, t, WIDTH-1, HEIGHT-1);
        if (time < 1<<30)
            return time;
    }
    return -1;
}

int part2(FILE *in)
{
    Blizzard blizzards[WIDTH*HEIGHT] = {0};
    int count = read(in, blizzards);
    int maps[WIDTH*HEIGHT*LOOPING] = {0};
    generateMaps(blizzards, count, maps);
    int time;
    // Go to goal
    for (int t = 1; t <= LOOPING; t++)
    {
        time = bfs(maps, 0, 0, t, WIDTH-1, HEIGHT-1);
        if (time < 1<<30)
            break;
    }
    // Get the snacks
    for (int t = 1; t <= LOOPING; t++)
    {
        int new = bfs(maps, WIDTH-1, HEIGHT-1, time+t, 0, 0);
        if (new < 1<<30)
        {
            time = new;
            break;
        }
    }
    // Go to goal again
    for (int t = 1; t <= LOOPING; t++)
    {
        int new = bfs(maps, 0, 0, time+t, WIDTH-1, HEIGHT-1);
        if (new < 1<<30)
        {
            time = new;
            break;
        }
    }
    return time;
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

void generateMaps(Blizzard *blizzards, int count, int *maps)
{
    for (int t = 0; t < LOOPING; t++)
    {
        for (int i = 0; i < count; i++)
            maps[_(blizzards[i].x, blizzards[i].y, t)] = blizzards[i].direction+1;
        iterate(blizzards, count);
    }
}

int doesEdgeExist(int *maps, int sx, int sy, int st, int ex, int ey, int et)
{
    // An edge exists from (x,y,t) -> (x+-1,y+-1,(t+1)%LOOPING) with cost 1 IF the new position is free
    // AND from (WIDTH-1,HEIGHT-1,t1) -> (WIDTH-1,HEIGHT-1,t2), for any t1,t2 with cost 0
    // The start node is believed to be at (0,0,1), so 1 minute has already passed
    // The end node is at (WIDTH-1,HEIGHT-1,t) for any given t
    if (et - st != 1)
        return 0;
    if (abs(sx-ex) > 1 || abs(sy-ey) > 1)
        return 0;
    if (ex < 0 || ey < 0 || ex >= WIDTH || ey >= HEIGHT)
        return 0;
    if (maps[_(ex,ey,et)])
        return 0;
    return 1;
}

void try(int x, int y, int t, int nx, int ny, int nt, int *lowestDistances, int currenDistance, int *out, int *current, int *currentCount, int *maps)
{
    if (doesEdgeExist(maps, x, y, t, nx, ny, nt) &&
            currenDistance + 1 < lowestDistances[_(nx, ny, nt)] &&
            !out[_(nx, ny, nt)])
    {
        if (lowestDistances[_(nx, ny, nt)] >= 1<<30)
            current[(*currentCount)++] = _(nx, ny, nt);
        lowestDistances[_(nx, ny, nt)] = currenDistance+1;
    }
}

int bfs(int *maps, int startX, int startY, int startT, int endX, int endY)
{
    int *lowestDistances = malloc(WIDTH*HEIGHT*LOOPING*sizeof(*lowestDistances));
    for (int i = 0; i < WIDTH*HEIGHT*LOOPING; i++)
        lowestDistances[i] = 1<<30;
    lowestDistances[_(startX,startY,startT)] = startT;

    int *out = calloc(WIDTH*HEIGHT*LOOPING, sizeof(*out));
    int *current = malloc(WIDTH*HEIGHT*LOOPING*sizeof(*current));
    current[0] = _(startX,startY,startT);
    int currentCount = 1;
    int currentStart = 0;

    int ret = 1<<30;
    while(1)
    {
        if (currentStart >= currentCount)
            goto end;
        int currenIndex = current[currentStart++];
        int currenDistance = lowestDistances[currenIndex];
        if (currenIndex == -1)
            goto end;

        int x = currenIndex % WIDTH;
        int y = (currenIndex / WIDTH) % HEIGHT;
        int t = currenIndex / WIDTH / HEIGHT;

        if (x == endX && y == endY)
        {
            // Found my goal!
            // +1 because I need to go one step further
            ret = currenDistance+1;
            goto end;
        }

        out[currenIndex] = 1;

        // Right
        try(x, y, t, x+1, y, t+1, lowestDistances, currenDistance, out, current, &currentCount, maps);
        // Left
        try(x, y, t, x-1, y, t+1, lowestDistances, currenDistance, out, current, &currentCount, maps);
        // Down
        try(x, y, t, x, y+1, t+1, lowestDistances, currenDistance, out, current, &currentCount, maps);
        // Up
        try(x, y, t, x, y-1, t+1, lowestDistances, currenDistance, out, current, &currentCount, maps);
        // Stationary
        try(x, y, t, x, y, t+1, lowestDistances, currenDistance, out, current, &currentCount, maps);
    }
end:
    free(lowestDistances);
    free(out);
    free(current);
    return ret;
}

void printMap(int *maps, int t)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
            switch(maps[_(x,y,t)])
            {
                case 0:
                    printf(".");
                    break;
                case 1:
                    printf(">");
                    break;
                case 2:
                    printf("v");
                    break;
                case 3:
                    printf("<");
                    break;
                case 4:
                    printf("^");
                    break;
                default:
                    printf("%d", maps[_(x,y,t)]);
                    break;
            }
        printf("\n");
    }
}

