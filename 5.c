#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define CRATES 9
#define CRATE_HEIGHT 8

int part1(FILE *in);
int part2(FILE *in);
char **parse(FILE *in);
void execute(char **crates, int n, int s, int d);
void execute9001(char **crates, int n, int s, int d);
void move(char **crates, int s, int d);
void print(char **crates);

int main()
{
    FILE *in = fopen("in5", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    char **parsed = parse(in);
    
    int n, s, d;
    while (fscanf(in, "move %d from %d to %d\n", &n, &s, &d) == 3)
        execute(parsed, n, s, d);
    print(parsed);

    for (int i = 0; i < CRATES; i++)
        free(parsed[i]);
    free(parsed);
    return -1;
}

int part2(FILE *in)
{
    char **parsed = parse(in);
    
    int n, s, d;
    while (fscanf(in, "move %d from %d to %d\n", &n, &s, &d) == 3)
        execute9001(parsed, n, s, d);
    print(parsed);

    for (int i = 0; i < CRATES; i++)
        free(parsed[i]);
    free(parsed);
    return -1;
}

char **parse(FILE *in)
{
    char c;
    char **res = malloc(CRATES*sizeof(*res));
    for (int i = 0; i < CRATES; i++)
        res[i] = calloc(CRATES*CRATE_HEIGHT+1, sizeof(*res[i]));
    int index = 0;
    int line = 0;
    while ((c = fgetc(in)) != EOF && c != '9')
    {
        if (c == ' ')
        {
            // Nothing
            index++;
            for (int i = 0; i < 3; i++)
                if (fgetc(in) == '\n')
                {
                    line++;
                    index = 0;
                }
            continue;
        }
        if (c == '\n')
        {
            // End of line
            if (index == 0)
                // End of input
                break;
            line++;
            index = 0;
            continue;
        }
        if (c != '[')
        {
            fprintf(stderr, "ERROR\n");
            exit(1);
        }
        c = fgetc(in);
        res[index][CRATE_HEIGHT-line-1] = c;
        index++;
        if (fgetc(in) == '\n' || fgetc(in) == '\n')
        {
            line++;
            index = 0;
        }
    }
    return res;
}

void execute(char **crates, int n, int s, int d)
{
    s--;
    d--;
    for (int i = 0; i < n; i++)
        move(crates, s, d);
}

void execute9001(char **crates, int n, int s, int d)
{
    s--;
    d--;
    int lenS = strlen(crates[s]);
    int lenD = strlen(crates[d]);
    for (int i = 0; i < n; i++)
        crates[d][lenD+i] = crates[s][lenS-n+i];
    crates[d][lenD+n] = '\0';
    crates[s][lenS-n] = '\0';
}

void move(char **crates, int s, int d)
{
    crates[d][strlen(crates[d])] = crates[s][strlen(crates[s])-1];
    crates[s][strlen(crates[s])-1] = '\0';
}

void print(char **crates)
{
    for (int i = 0; i < CRATES; i++)
        printf("%c", crates[i][strlen(crates[i])-1]);
    printf("\n");
}

