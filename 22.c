#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define WIDTH 150
#define HEIGHT 200
#define CUBESIZE 50
//#define WIDTH 12
//#define HEIGHT 16
//#define CUBESIZE 4

#define _(x,y) ((y)*(WIDTH) + (x))

int part1(FILE *in);
int part2(FILE *in);
void readMap(FILE *in, int *buff);
void move(int *map, int *x, int *y, int dir);
void move2(int *map, int *x, int *y, int *dir);
void runCommands(FILE *in, int *map, int *x, int *y, int *dir, int part);
int calculatePassword(int x, int y, int dir);
void print(int *map);

int whereWasI[WIDTH*HEIGHT] = {0};

int main()
{
    FILE *in = fopen("in22", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int map[WIDTH*HEIGHT];
    readMap(in, map);
    int x;
    int y = 0;
    for (x = 0; x < WIDTH; x++)
        if (!map[_(x,y)])
            break;
    int dir = 0;
    runCommands(in, map, &x, &y, &dir, 1);
    return calculatePassword(x, y, dir);
}

int part2(FILE *in)
{
    int map[WIDTH*HEIGHT];
    for (int i = 0; i < WIDTH*HEIGHT; i++)
        whereWasI[i] = 0;
    readMap(in, map);
    int x;
    int y = 0;
    for (x = 0; x < WIDTH; x++)
        if (!map[_(x,y)])
            break;
    int dir = 0;
    runCommands(in, map, &x, &y, &dir, 2);
    return calculatePassword(x, y, dir);
}

void readMap(FILE *in, int *buff)
{
    int x = 0, y = 0;
    for (int i = 0; i < WIDTH*HEIGHT; i++)
        buff[i] = -2;
    while(1)
    {
        switch(fgetc(in))
        {
            case ' ':
                buff[_(x++,y)] = -1;
                break;
            case '.':
                buff[_(x++,y)] = 0;
                break;
            case '#':
                buff[_(x++,y)] = 1;
                break;
            case '\n':
                if (x == 0)
                    return;
                x = 0;
                y++;
                break;
        }
    }
}

void move(int *map, int *x, int *y, int dir)
{
    whereWasI[_(*x,*y)] = dir+1;
    int oldX = *x;
    int oldY = *y;
    switch(dir)
    {
        case 0:
            // Right
            (*x)++;
            if (*x >= WIDTH || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                int i;
                for (i = 0; i < WIDTH; i++)
                    if (map[_(i,*y)] >= 0)
                        break;
                *x = i;
            }
            if (map[_(*x,*y)])
                // Wall!
                *x = oldX;
            break;
        case 1:
            // Down
            (*y)++;
            if (*y >= HEIGHT || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                int i;
                for (i = 0; i < HEIGHT; i++)
                    if (map[_(*x,i)] >= 0)
                        break;
                *y = i;
            }
            if (map[_(*x,*y)])
                // Wall!
                *y = oldY;
            break;
        case 2:
            // Left
            (*x)--;
            if (*x < 0 || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                int i;
                for (i = WIDTH-1; i >= 0; i--)
                    if (map[_(i,*y)] >= 0)
                        break;
                *x = i;
            }
            if (map[_(*x,*y)])
                // Wall!
                *x = oldX;
            break;
        case 3:
            // Up
            (*y)--;
            if (*y < 0 || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                int i;
                for (i = HEIGHT-1; i >= 0; i--)
                    if (map[_(*x,i)] >= 0)
                        break;
                *y = i;
            }
            if (map[_(*x,*y)])
                // Wall!
                *y = oldY;
            break;
    }
}

void runCommands(FILE *in, int *map, int *x, int *y, int *dir, int part)
{
    int c;
    int currentNumber = 0;
    while ((c = fgetc(in)) != EOF && c != '\n')
    {
        if (c >= '0' && c <= '9')
            currentNumber = currentNumber*10 + c-'0';
        else
        {
            for (int i = 0; i < currentNumber; i++)
            {
                if (part == 1)
                    move(map, x, y, *dir);
                else
                    move2(map, x, y, dir);
            }
            currentNumber = 0;
            if (c == 'L')
            {
                (*dir)--;
                while (*dir < 0)
                    *dir += 4;
            }
            else if (c == 'R')
                *dir = ((*dir)+1)%4;
        }
    }
    for (int i = 0; i < currentNumber; i++)
    {
        if (part == 1)
            move(map, x, y, *dir);
        else
            move2(map, x, y, dir);
    }
}

int calculatePassword(int x, int y, int dir)
{
    return (1000 * (y+1)) + (4 * (x+1)) + dir;
}

void print(int *map)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (map[_(x,y)] < 0)
                printf(" ");
            else if (map[_(x,y)] == 0)
            {
                if (whereWasI[_(x,y)])
                    switch(whereWasI[_(x,y)])
                    {
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
                    }
                else
                    printf(".");
            }
            else
                printf("#");
        }
        printf("\n");
    }
    printf("\n");
}

void move2(int *map, int *x, int *y, int *dir)
{
    whereWasI[_(*x,*y)] = *dir+1;
    int oldX = *x;
    int oldY = *y;
    int oldDir = *dir;
    switch(*dir)
    {
        case 0:
            // Right
            (*x)++;
            if (*x >= WIDTH || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                if (*y < CUBESIZE)
                {
                    // 2 -> 5, right -> left, y -> -y
                    *x = 2*CUBESIZE-1;
                    *y = 3*CUBESIZE-1-*y;
                    *dir = 2;
                }
                else if (*y < 2*CUBESIZE)
                {
                    // 3 -> 2, right -> up
                    *x = 2*CUBESIZE+(*y-CUBESIZE);
                    *y = CUBESIZE-1;
                    *dir = 3;
                }
                else if (*y < 3*CUBESIZE)
                {
                    // 5 -> 2, right -> left
                    *x = 3*CUBESIZE-1;
                    *y = CUBESIZE-1-(*y-2*CUBESIZE);
                    *dir = 2;
                }
                else
                {
                    // 6 -> 5, right -> up
                    *x = CUBESIZE+(*y-3*CUBESIZE);
                    *y = 3*CUBESIZE-1;
                    *dir = 3;
                }
            }
            if (map[_(*x,*y)])
            {
                // Wall!
                *x = oldX;
                *y = oldY;
                *dir = oldDir;
            }
            break;
        case 1:
            // Down
            (*y)++;
            if (*y >= HEIGHT || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                if (*x < CUBESIZE)
                {
                    // 6 -> 2, down -> down
                    *x += 2*CUBESIZE;
                    *y = 0;
                }
                else if (*x < 2*CUBESIZE)
                {
                    // 5 -> 6, down -> left
                    *y = 3*CUBESIZE+(*x-CUBESIZE);
                    *x = CUBESIZE-1;
                    *dir = 2;
                }
                else
                {
                    // 2 -> 3, down -> left
                    *y = CUBESIZE+(*x-2*CUBESIZE);
                    *x = 2*CUBESIZE-1;
                    *dir = 2;
                }
            }
            if (map[_(*x,*y)])
            {
                // Wall!
                *x = oldX;
                *y = oldY;
                *dir = oldDir;
            }
            break;
        case 2:
            // Left
            (*x)--;
            if (*x < 0 || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                if (*y < CUBESIZE)
                {
                    // 1 -> 4, left -> right
                    *x = 0;
                    *y = 3*CUBESIZE-1-*y;
                    *dir = 0;
                }
                else if (*y < 2*CUBESIZE)
                {
                    // 3 -> 4, left -> down
                    *x = 0+*y-CUBESIZE;
                    *y = 2*CUBESIZE;
                    *dir = 1;
                }
                else if (*y < 3*CUBESIZE)
                {
                    // 4 -> 1, left -> right
                    *x = CUBESIZE;
                    *y = CUBESIZE-1-(*y-2*CUBESIZE);
                    *dir = 0;
                }
                else
                {
                    // 6 -> 1, left -> down
                    *x = CUBESIZE+(*y-3*CUBESIZE);
                    *y = 0;
                    *dir = 1;
                }
            }
            if (map[_(*x,*y)])
            {
                // Wall!
                *x = oldX;
                *y = oldY;
                *dir = oldDir;
            }
            break;
        case 3:
            // Up
            (*y)--;
            if (*y < 0 || map[_(*x,*y)] < 0)
            {
                // Out of bounds!
                if (*x < CUBESIZE)
                {
                    // 4 -> 3, up -> right
                    *y = CUBESIZE+*x;
                    *x = CUBESIZE;
                    *dir = 0;
                }
                else if (*x < 2*CUBESIZE)
                {
                    // 1 -> 6, up -> right
                    *y = 3*CUBESIZE+(*x-CUBESIZE);
                    *x = 0;
                    *dir = 0;
                }
                else
                {
                    // 2 -> 6, up -> up
                    *x -= 2*CUBESIZE;
                    *y = 4*CUBESIZE-1;
                }
            }
            if (map[_(*x,*y)])
            {
                // Wall!
                *x = oldX;
                *y = oldY;
                *dir = oldDir;
            }
            break;
    }
}

