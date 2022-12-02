#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int part1(FILE *in);
int part2(FILE *in);

int main()
{
    FILE *in = fopen("in2", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int c;
    int score = 0;
    int scoreCalc[] =
    {
        // Opponent rock
        3+1,
        6+2,
        0+3,
        // Opponent paper
        0+1,
        3+2,
        6+3,
        // Opponent scissors
        6+1,
        0+2,
        3+3
    };
    while ((c = fgetc(in)) != EOF)
    {
        int opponent = c - 'A';
        if ((c = fgetc(in)) != ' ')
        {
            fprintf(stderr, "Unexpected character: '%c'\n", c);
            exit(1);
        }
        int me = fgetc(in) - 'X';
        if ((c = fgetc(in)) != '\n')
        {
            fprintf(stderr, "Unexpected character: '%c'\n", c);
            exit(1);
        }
        score += scoreCalc[opponent * 3 + me];
    }
    return score;
}

int part2(FILE *in)
{
    int c;
    int score = 0;
    int scoreCalc[] =
    {
        // Opponent rock
        0+3,
        3+1,
        6+2,
        // Opponent paper
        0+1,
        3+2,
        6+3,
        // Opponent scissors
        0+2,
        3+3,
        6+1
    };
    while ((c = fgetc(in)) != EOF)
    {
        int opponent = c - 'A';
        if ((c = fgetc(in)) != ' ')
        {
            fprintf(stderr, "Unexpected character: '%c'\n", c);
            exit(1);
        }
        int me = fgetc(in) - 'X';
        if ((c = fgetc(in)) != '\n')
        {
            fprintf(stderr, "Unexpected character: '%c'\n", c);
            exit(1);
        }
        score += scoreCalc[opponent * 3 + me];
    }
    return score;
}

