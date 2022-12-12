#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int part1(FILE *in);
int part2(FILE *in);
int execute(FILE *in, int *cycle, int *x);
void draw(int cycle, int x);
int abs(int x);

int main()
{
    FILE *in = fopen("in10", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int cycle = 1;
    int x = 1;
    int res = 0;
    for (; cycle < 222;)
    {
        if (cycle%40 == 20 || cycle%40 == 19)
        {
            int mult = cycle%20 == 0 ? cycle : cycle+1;
            res += mult * x;
        }
        if (!execute(in, &cycle, &x))
        {
            fprintf(stderr, "No commands left!\n");
            exit(2);
        }
    }
    return res;
}

int part2(FILE *in)
{
    int cycle = 1;
    int x = 1;
    while(1)
    {
        draw(cycle, x);
        if (cycle%40 == 1)
            printf("\n");
        int oldx = x;
        int ret;
        if (!(ret = execute(in, &cycle, &x)))
            break;
        if (ret == 2)
        {
            draw(cycle-1, oldx);
            if ((cycle-1)%40 == 1)
                printf("\n");
        }
    }
    return 0;
}

int execute(FILE *in, int *cycle, int *x)
{
    int c = fgetc(in);
    switch(c)
    {
        case 'n':
            (*cycle)++;
            for (int i = 0; i < (int)strlen("oop\n"); i++)
                fgetc(in);
            return 1;
        case 'a':
            *cycle+=2;
            int n = 0;
            if (fscanf(in, "ddx %d\n", &n) != 1)
            {
                fprintf(stderr, "Invalid file!\n");
                exit(1);
            }
            *x += n;
            return 2;
        case EOF:
        case '\n':
            // End of file
            return 0;
        default:
            fprintf(stderr, "Invalid file!\n");
            exit(1);
    }
}

void draw(int cycle, int x)
{
    if (abs((cycle-1)%40 - x) <= 1)
        printf("#");
    else
        printf(".");
}

int abs(int x)
{
    return x < 0 ? -x : x;
}

