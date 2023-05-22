#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define INITIAL_SIZE 73
//#define INITIAL_SIZE 7
#define ROUNDS 10

#define _(x,y) ((y)*((INITIAL_SIZE)+2*(ROUNDS))+(x))
#define abs(x) ((x) < 0 ? -(x) : (x))

int part1(FILE *in);
int part2(FILE *in);
void read(FILE *in, int *buff, int offset);
void iterate(int *map, int direction);
void move(int *x, int *y, int direction);
int isFree(int *map, int x, int y, int direction);
void smallestRectangle(int *map, int *x1, int *x2, int *y1, int *y2);
int countEmptyGround(int *map, int x1, int x2, int y1, int y2);
void print(int *map);
int wantsToMove(int *map, int x, int y);

int main()
{
    FILE *in = fopen("in23", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int map[(INITIAL_SIZE+2*ROUNDS)*(INITIAL_SIZE+2*ROUNDS)] = {0};
    read(in, map, ROUNDS);
    //printf("Initial map:\n");
    //print(map);
    for (int i = 0; i < ROUNDS; i++)
    {
        //printf("Iteration %d:\n", i);
        iterate(map, i%4);
    }
    int x1, x2, y1, y2;
    smallestRectangle(map, &x1, &x2, &y1, &y2);
    return countEmptyGround(map, x1, x2, y1, y2);
}

int part2(FILE *in)
{
    return in == NULL ? -3 : -2;
}

void read(FILE *in, int *buff, int offset)
{
    for (int y = 0; y < INITIAL_SIZE; y++)
    {
        for (int x = 0; x < INITIAL_SIZE; x++)
            buff[_(x+offset, y+offset)] = (fgetc(in) == '#');
        if (fgetc(in) != '\n')
            fprintf(stderr, "Expected linebreak at end of line!\n");
    }
}

void iterate(int *map, int direction)
{
    for (int y = 0; y < INITIAL_SIZE+2*ROUNDS; y++)
        for (int x = 0; x < INITIAL_SIZE+2*ROUNDS; x++)
            if (map[_(x,y)] == 1 && wantsToMove(map, x, y))
            {
                for (int i = 0; i < 4; i++)
                    if (isFree(map, x, y, (direction+i)%4))
                    {
                        int nx = x, ny = y;
                        move(&nx, &ny, (direction+i)%4);
                        if (!map[_(nx, ny)])
                        {
                            // Proposing to move to the new position
                            map[_(nx, ny)] = (direction+i)%4 + 4;
                            map[_(x, y)] = -1;
                        }
                        else if (map[_(nx, ny)] == -2)
                            // Already two or more elfes wanted to move here!
                            break;
                        else
                        {
                            // Already one other elf wanted to move here!
                            // Get the old position of the other elf
                            int ox = nx, oy = ny;
                            move(&ox, &oy, map[_(nx, ny)]);
                            // Undo the proposal of the other elf
                            map[_(ox, oy)] = 1;
                            // Mark the field as burned
                            map[_(nx, ny)] = -2;
                        }
                        break;
                    }
            }
    //printf("After proposals:\n");
    //print(map);
    // Do the valid proposals
    for (int y = 0; y < INITIAL_SIZE+2*ROUNDS; y++)
        for (int x = 0; x < INITIAL_SIZE+2*ROUNDS; x++)
            if (map[_(x,y)] > 3)
            {
                // Valid proposal - get old position
                int ox = x, oy = y;
                move(&ox, &oy, map[_(x,y)]);
                // Move
                map[_(ox, oy)] = 0;
                map[_(x, y)] = 1;
            }
            else if (map[_(x,y)] < -1)
                // Free burned fields
                map[_(x,y)] = 0;
    //printf("After moving:\n");
    //print(map);
}

void move(int *x, int *y, int direction)
{
    switch(direction)
    {
        case 0:
        case 5:
            (*y)--;
            break;
        case 1:
        case 4:
            (*y)++;
            break;
        case 2:
        case 7:
            (*x)--;
            break;
        case 3:
        case 6:
            (*x)++;
            break;
    }
}

int isFree(int *map, int x, int y, int direction)
{
    switch(direction)
    {
        case 0:
            return abs(map[_(x,y-1)]) != 1 &&
                abs(map[_(x-1,y-1)]) != 1 &&
                abs(map[_(x+1,y-1)]) != 1;
        case 1:
            return abs(map[_(x,y+1)]) != 1 &&
                abs(map[_(x-1,y+1)]) != 1 &&
                abs(map[_(x+1,y+1)]) != 1;
        case 2:
            return abs(map[_(x-1,y)]) != 1 &&
                abs(map[_(x-1,y-1)]) != 1 &&
                abs(map[_(x-1,y+1)]) != 1;
        case 3:
            return abs(map[_(x+1,y)]) != 1 &&
                abs(map[_(x+1,y-1)]) != 1 &&
                abs(map[_(x+1,y+1)]) != 1;
    }
    return -1;
}

void smallestRectangle(int *map, int *x1, int *x2, int *y1, int *y2)
{
    for (int x = 0;; x++)
    {
        int y;
        for (y = 0; y < INITIAL_SIZE+2*ROUNDS; y++)
            if (map[_(x,y)])
                break;
        if (y != INITIAL_SIZE+2*ROUNDS)
        {
            // Found an elf!
            *x1 = x;
            break;
        }
    }

    for (int x = INITIAL_SIZE+2*ROUNDS-1;; x--)
    {
        int y;
        for (y = 0; y < INITIAL_SIZE+2*ROUNDS; y++)
            if (map[_(x,y)])
                break;
        if (y != INITIAL_SIZE+2*ROUNDS)
        {
            // Found an elf!
            *x2 = x;
            break;
        }
    }

    for (int y = 0;; y++)
    {
        int x;
        for (x = 0; x < INITIAL_SIZE+2*ROUNDS; x++)
            if (map[_(x,y)])
                break;
        if (x != INITIAL_SIZE+2*ROUNDS)
        {
            // Found an elf!
            *y1 = y;
            break;
        }
    }

    for (int y = INITIAL_SIZE+2*ROUNDS-1;; y--)
    {
        int x;
        for (x = 0; x < INITIAL_SIZE+2*ROUNDS; x++)
            if (map[_(x,y)])
                break;
        if (x != INITIAL_SIZE+2*ROUNDS)
        {
            // Found an elf!
            *y2 = y;
            break;
        }
    }
}

int countEmptyGround(int *map, int x1, int x2, int y1, int y2)
{
    int count = 0;
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
            count += !map[_(x,y)];
    return count;
}

void print(int *map)
{
    for (int y = 0; y < INITIAL_SIZE+2*ROUNDS; y++)
    {
        for (int x = 0; x < INITIAL_SIZE+2*ROUNDS; x++)
        {
            switch(map[_(x,y)])
            {
                case 0:
                    printf(".");
                    break;
                case 1:
                    printf("#");
                    break;
                case -1:
                    printf("o");
                    break;
                case 4:
                    printf("^");
                    break;
                case 5:
                    printf("v");
                    break;
                case 6:
                    printf("<");
                    break;
                case 7:
                    printf(">");
                    break;
                case -2:
                    printf("x");
                    break;
            }
        }
        printf("\n");
    }
}

int wantsToMove(int *map, int x, int y)
{
    return abs(map[_(x-1,y-1)]) == 1 ||
        abs(map[_(x-1,y)]) == 1 ||
        abs(map[_(x-1,y+1)]) == 1 ||
        abs(map[_(x,y+1)]) == 1 ||
        abs(map[_(x+1,y+1)]) == 1 ||
        abs(map[_(x+1,y)]) == 1 ||
        abs(map[_(x+1,y-1)]) == 1 ||
        abs(map[_(x,y-1)]) == 1;
}

