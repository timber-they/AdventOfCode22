#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define SIZE 99

#define _(x,y) ((SIZE)*(y)+(x))

int part1(FILE *in);
int part2(FILE *in);
int *read(FILE *in, int *buff);
void markVisible(int *field);
int countVisible(int *field);
int scenicScore(int *field, int x, int y);
int highestScore(int *field);

int main()
{
    FILE *in = fopen("in8", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int field[SIZE*SIZE];
    read(in, field);
    markVisible(field);
    return countVisible(field);
}

int part2(FILE *in)
{
    int field[SIZE*SIZE];
    read(in, field);
    return highestScore(field);
}

int *read(FILE *in, int *buff)
{
    for (int y = 0; y < SIZE; y++)
    {
        for (int x = 0; x < SIZE; x++)
        {
            int c = fgetc(in);
            if (c < '0' || c > '9')
            {
                fprintf(stderr, "Invalid character: %c\n", c);
                exit(1);
            }
            buff[_(x,y)] = c-'0';
        }
        if (fgetc(in) != '\n')
        {
            fprintf(stderr, "Expected line break!\n");
            exit(2);
        }
    }
    return buff;
}

void markVisible(int *field)
{
    for (int i = 0; i < SIZE; i++)
    {
        int maxLeft = -1,
            maxRight = -1,
            maxTop = -1,
            maxBottom = -1;
        for (int j = 0; j < SIZE; j++)
        {
            // i=y, j=x
            if (field[_(j,i)]%10 > maxLeft)
            {
                maxLeft = field[_(j,i)]%10;
                field[_(j,i)] += 10;
            }
            // i=y, j=-x
            if (field[_(SIZE-j-1,i)]%10 > maxRight)
            {
                maxRight = field[_(SIZE-j-1,i)]%10;
                field[_(SIZE-j-1,i)] += 10;
            }
            // i=x, j=y
            if (field[_(i,j)]%10 > maxTop)
            {
                maxTop = field[_(i,j)]%10;
                field[_(i,j)] += 10;
            }
            // i=x, j=-y
            if (field[_(i,SIZE-j-1)]%10 > maxBottom)
            {
                maxBottom = field[_(i,SIZE-j-1)]%10;
                field[_(i,SIZE-j-1)] += 10;
            }
        }
    }
}

int countVisible(int *field)
{
    int res = 0;
    for (int i = 0; i < SIZE*SIZE; i++)
        res += field[i] >= 10;
    return res;
}

int scenicScore(int *field, int x, int y)
{
    if (x == SIZE-1 || y == SIZE-1 || x == 0 || y == 0)
        return 0;
    int score = 1;
    for (int i = x+1; i < SIZE; i++)
        if (field[_(i,y)] >= field[_(x,y)] || i == SIZE-1)
        {
            score *= i-x;
            break;
        }
    for (int i = x-1; i >= 0; i--)
        if (field[_(i,y)] >= field[_(x,y)] || i == 0)
        {
            score *= x-i;
            break;
        }
    for (int i = y+1; i < SIZE; i++)
        if (field[_(x,i)] >= field[_(x,y)] || i == SIZE-1)
        {
            score *= i-y;
            break;
        }
    for (int i = y-1; i >= 0; i--)
        if (field[_(x,i)] >= field[_(x,y)] || i == 0)
        {
            score *= y-i;
            break;
        }
    return score;
}

int highestScore(int *field)
{
    int max = 0;
    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++)
        {
            int score = scenicScore(field, x, y);
            if (score > max)
                max = score;
        }
     return max;
}

