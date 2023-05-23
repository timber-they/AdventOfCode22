#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MAX_LENGTH 21
#define NUMBERS 118
//#define NUMBERS 13
//#define NUMBERS 2

typedef int SNAFU[MAX_LENGTH+1];

int part1(FILE *in);
int part2(FILE *in);
void read(FILE *in, SNAFU *buff);
// Result is stored in b
void sum(SNAFU a, SNAFU b);
void print(SNAFU number);

int main()
{
    FILE *in = fopen("in25", "r");

    part1(in);
    rewind(in);
    part2(in);

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    SNAFU numbers[NUMBERS] = {0};
    read(in, numbers);
    for (int i = 1; i < NUMBERS; i++)
        sum(numbers[i-1], numbers[i]);
    printf("Part1: ");
    print(numbers[NUMBERS-1]);
    printf("\n");
    return 0;
}

int part2(FILE *in)
{
    printf("Part2: Start the smoothie blender!\n");
    return in == NULL ? -3 : -2;
}

void read(FILE *in, SNAFU *buff)
{
    for (int i = 0; i < NUMBERS; i++)
    {
        char line[MAX_LENGTH+1] = {0};
        fgets(line, MAX_LENGTH+1, in);
        char *newLine = strchr(line, '\n');
        if (newLine == NULL)
            fprintf(stderr, "Expected new line! (i=%d)\n", i);
        *newLine = '\0';
        int transform[1<<8] = {0};
        transform['='] = -2;
        transform['-'] = -1;
        transform['0'] = 0;
        transform['1'] = 1;
        transform['2'] = 2;
        for (int j = strlen(line)-1; j >= 0; j--)
            // Least significant byte first
            buff[i][strlen(line)-1-j] = transform[(int)line[j]];
        for (int j = strlen(line); j < MAX_LENGTH; j++)
            buff[i][j] = 0;
    }
}

void sum(SNAFU a, SNAFU b)
{
    // Add a to b
    //print(a);
    //printf(" + ");
    //print(b);
    int overhead = 0;
    int i;
    for (i = 0; i < MAX_LENGTH; i++)
    {
        b[i] += a[i] + overhead;
        overhead = 0;
        while (b[i] > 2)
        {
            overhead++;
            b[i] -= 5;
        }
        while (b[i] < -2)
        {
            overhead--;
            b[i] += 5;
        }
    }
    b[i] += overhead;
    //printf(" = ");
    //print(b);
    //printf("\n");
}

void print(SNAFU number)
{
    int e;
    for (e = MAX_LENGTH; number[e] == 0; e--)
        ;
    int buff[5] = {0};
    int *transform = buff+2;
    transform[-2] = '=';
    transform[-1] = '-';
    transform[0] = '0';
    transform[1] = '1';
    transform[2] = '2';
    for (; e >= 0; e--)
        printf("%c", transform[number[e]]);
}

