#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int part1(FILE *in);
int part2(FILE *in);
int getPriority(char c);

int main()
{
    FILE *in = fopen("in3", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    char *line = NULL;
    size_t n = 0;
    int sum = 0;
    while (getline(&line, &n, in) > 1)
    {
        int len = strlen(line);
        char doubleCharacter;
        for (int i = 0; i < len / 2; i++)
            for (int j = 0; j < len / 2; j++)
                if (line[i] == line[len/2+j])
                {
                    doubleCharacter = line[i];
                    goto out;
                }
        fprintf(stderr, "ERROR - no double item! Line: %s\n", line);
        exit(1);
out:
        sum += getPriority(doubleCharacter);
    }
    free(line);
    return sum;
}

int part2(FILE *in)
{
    char *line1 = NULL;
    char *line2 = NULL;
    char *line3 = NULL;
    size_t n = 0;
    int sum = 0;
    while (getline(&line1, &n, in) > 1 &&
        getline(&line2, &n, in) > 1 &&
        getline(&line3, &n, in) > 1)
    {
        for (size_t i = 0; i < strlen(line1); i++)
            for (size_t j = 0; j < strlen(line2); j++)
                if (line1[i] == line2[j])
                    for (size_t k = 0; k < strlen(line3); k++)
                        if (line1[i] == line3[k])
                        {
                            sum += getPriority(line1[i]);
                            goto breaking;
                        }
breaking:;
    }
    free(line1);
    free(line2);
    free(line3);
    return sum;
}

int getPriority(char c)
{
    return c >= 'a' && c <= 'z'
        ? c - 'a' + 1
        : c >= 'A' && c <= 'Z'
        ? c - 'A' + 27
        : -1;
}

