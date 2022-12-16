#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define VALVES 51
#define TIME 30

#define _(from,to) ((to)*(VALVES) + (from))
#define __(openKey,leftTime,current) ((openKey)*(TIME)*(VALVES) + (leftTime)*(VALVES) + (current))

int part1(FILE *in);
int part2(FILE *in);
int readValves(FILE *in, int *flowRates, int *tunnels);
int mostGas(int *flowRates, int *tunnels, int *memory, int current, int leftTime, int *open);
int getOpenKey(int *flowRates, int *open);
int maxOpenKey(int *flowRates);

int valveNames[VALVES];

int main()
{
    FILE *in = fopen("in16", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int flowRates[VALVES] = {0};
    int tunnels[VALVES*VALVES] = {0};
    int current = readValves(in, flowRates, tunnels);
    int openKeyCount = maxOpenKey(flowRates)+1;
    int *memory = malloc(openKeyCount*TIME*VALVES*sizeof(*memory));
    for (int i = 0; i < openKeyCount*TIME*VALVES; i++)
        memory[i] = -1;
    int open[VALVES] = {0};
    int ret = mostGas(flowRates, tunnels, memory, current, TIME, open);
    free(memory);
    return ret;
}

int part2(FILE *in)
{
    return in == NULL ? -3 :  -2;
}

int readValves(FILE *in, int *flowRates, int *tunnels)
{
    int ret = -1;
    for (int i = 0; i < VALVES; i++)
    {
        char s1, s2;
        long curr = ftell(in);
        char throwAway[50];
        fseek(in, curr, SEEK_SET);
        if (fscanf(in, "Valve %c%c has flow rate=%d; tunnels lead to valves %[^\n]\n", &s1, &s2, flowRates+i, throwAway) != 4)
        {
            fseek(in, curr, SEEK_SET);
            if (fscanf(in, "Valve %c%c has flow rate=%d; tunnel leads to valve %[^\n]\n", &s1, &s2, flowRates+i, throwAway) != 4)
                fprintf(stderr, "ERROR 1 - next char: %c\n", fgetc(in));
        }
        valveNames[i] = (s1-'A')*26 + s2-'A';
        if (valveNames[i] == 0)
        {
            ret = i;
        }
    }
    rewind(in);
    for (int i = 0; i < VALVES; i++)
    {
        if (fscanf(in, "Valve %*c%*c has flow rate=%*d; tunnels lead to valve") != 0)
            fprintf(stderr, "ERROR 2\n");
        int c;
        int valve = -1;
        while ((c = fgetc(in)) != '\n')
        {
            switch(c)
            {
                case 's':
                    break;
                case ' ':
                    valve = -1;
                    break;
                case ',':
                    for (int j = 0; j < VALVES; j++)
                        if (valveNames[j] == valve)
                        {
                            tunnels[_(i,j)] = 1;
                            break;
                        }
                    break;
                default:
                    if (valve == -1)
                        valve = c-'A';
                    else
                        valve = valve * 26 + c-'A';
                    break;
            }
        }
        for (int j = 0; j < VALVES; j++)
            if (valveNames[j] == valve)
            {
                tunnels[_(i,j)] = 1;
                break;
            }
    }
    return ret;
}

int mostGas(int *flowRates, int *tunnels, int *memory, int current, int leftTime, int *open)
{
    int openKey = getOpenKey(flowRates, open);
    if (leftTime < 0)
        return -(1<<30);
    if (leftTime <= 0)
        return 0;
    if (memory[__(openKey,leftTime,current)] >= 0)
        return memory[__(openKey,leftTime,current)];

    int max = 0;
    int oldState = open[current];

    // Ignoring that valve
    for (int i = 0; i < VALVES; i++)
    {
        if (tunnels[_(current, i)])
        {
            int new = mostGas(flowRates, tunnels, memory, i, leftTime-1, open);
            if (new > max)
                max = new;
        }
    }
    open[current] = oldState;

    // Already open or useless to open
    if (open[current] || !flowRates[current])
        return (memory[__(openKey,leftTime,current)]=max);

    // Opening it
    open[current] = 1;
    int new = mostGas(flowRates, tunnels, memory, current, leftTime-1, open) + (leftTime-1)*flowRates[current];
    if (new > max)
        max = new;
    open[current] = oldState;
    return (memory[__(openKey,leftTime,current)]=max);
}

int getOpenKey(int *flowRates, int *open)
{
    int key = 0;
    for (int i = 0; i < VALVES; i++)
        if (flowRates[i])
            key = (key<<1)+open[i];
    return key;
}

int maxOpenKey(int *flowRates)
{
    int open[VALVES];
    for (int i = 0; i < VALVES; i++)
        open[i] = 1;
    return getOpenKey(flowRates, open);
}

