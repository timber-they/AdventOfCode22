#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int part1(FILE *in);
int part2(FILE *in);
int allDifferent(int *buff, int size);

int main()
{
    FILE *in = fopen("in6", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int buff[4] = {0};
    int index = 0;
    while (index < 4 || !allDifferent(buff, 4))
        buff[(index++)%4] = fgetc(in);
    return index;
}

int part2(FILE *in)
{
    int buff[14] = {0};
    int index = 0;
    while (index < 14 || !allDifferent(buff, 14))
        buff[(index++)%14] = fgetc(in);
    return index;
}

int allDifferent(int *buff, int size)
{
    for (int i = 0; i < size - 1; i++)
        for (int j = i+1; j < size; j++)
            if (buff[i] == buff[j])
                return 0;
    return 1;
}

