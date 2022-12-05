#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int part1(FILE *in);
int part2(FILE *in);

int main()
{
    FILE *in = fopen("in4", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int s1, e1, s2, e2;
    int count = 0;
    while (fscanf(in, "%d-%d,%d-%d\n", &s1, &e1, &s2, &e2) == 4)
        count += (s1 <= s2 && e1 >= e2) ||
                 (s2 <= s1 && e2 >= e1);
    return count;
}

int part2(FILE *in)
{
    int s1, e1, s2, e2;
    int count = 0;
    while (fscanf(in, "%d-%d,%d-%d\n", &s1, &e1, &s2, &e2) == 4)
        count += s1 <= e2 && s2 <= e1;
    return count;
}

