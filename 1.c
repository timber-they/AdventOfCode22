#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int part1(FILE *in);
int part2(FILE *in);

int main()
{
    FILE *in = fopen("in1", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int c;
    int num = 0;
    int sum = 0;
    int max = 0;
    while ((c = fgetc(in)) != EOF)
    {
        if (c >= '0' && c <= '9')
        {
            num = num * 10 + (c - '0');
            continue;
        }
        if (c != '\n')
        {
            fprintf(stderr, "Unexpected character: '%c'\n", c);
            exit(1);
        }
        if (num != 0)
        {
            // End of number
            sum += num;
            num = 0;
            continue;
        }
        // End of block, bc two line breaks in a row
        if (sum > max)
            max = sum;
        sum = 0;
    }
    return max;
}

int part2(FILE *in)
{
    int c;
    int num = 0;
    int sum = 0;
    int max1 = 0;
    int max2 = 0;
    int max3 = 0;
    while ((c = fgetc(in)) != EOF)
    {
        if (c >= '0' && c <= '9')
        {
            num = num * 10 + (c - '0');
            continue;
        }
        if (c != '\n')
        {
            fprintf(stderr, "Unexpected character: '%c'\n", c);
            exit(1);
        }
        if (num != 0)
        {
            // End of number
            sum += num;
            num = 0;
            continue;
        }
        // End of block, bc two line breaks in a row
        if (sum > max1)
        {
            max3 = max2;
            max2 = max1;
            max1 = sum;
        }
        else if (sum > max2)
        {
            max3 = max2;
            max2 = sum;
        }
        else if (sum > max3)
            max3 = sum;
        sum = 0;
    }
    return max1 + max2 + max3;
}

