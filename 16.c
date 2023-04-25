#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define VALVES 51
#define REL_VALVES 15
#define TIME 30

#define _(from,to) ((to)*(VALVES) + (from))
#define __(from,to) ((to)*(REL_VALVES) + (from))

int part1(FILE *in);
int part2(FILE *in);
int readValves(FILE *in, int *flowRates, int *tunnels);
void calculateCosts(int *tunnels, int *costs);
void reduceToRelevant(int *flowRates, int *costs, int *relFlowRates, int *relCosts, int start, int *costsFromStart);
void getRelevantGraph(FILE *in, int *relFlowRates, int *relCosts, int *costsFromStart);

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
    int flowRates[REL_VALVES];
    int costs[REL_VALVES*REL_VALVES];
    int costsFromStart[REL_VALVES];
    getRelevantGraph(in, flowRates, costs, costsFromStart);
    return -1;
}

int part2(FILE *in)
{
    if (in == NULL)
        return -3;
    return -2;
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

int calculateCost(int *tunnels, int *costs, int from, int to, int *visitted)
{
    if (costs[_(from, to)] >= 0)
        return costs[_(from, to)];
    if (from == to)
        return (costs[_(from, to)] = 0);
    if (costs[_(to, from)] >= 0)
        return (costs[_(from, to)] = costs[_(to, from)]);
    int min = 1<<30;
    visitted[from] = 1;
    for (int i = 0; i < VALVES; i++)
    {
        if (!tunnels[_(from,i)] || visitted[i])
            continue;
        int score = calculateCost(tunnels, costs, i, to, visitted) + 1;
        if (score < min)
            min = score;
    }
    visitted[from] = 0;
    return costs[_(from, to)] = min;
}

void calculateCosts(int *tunnels, int *costs)
{
    for (int i = 0; i < VALVES*VALVES; i++)
        costs[i] = -1;
    int visitted[VALVES] = {0};
    for (int i = 0; i < VALVES; i++)
        for (int j = 0; j < VALVES; j++)
            calculateCost(tunnels, costs, i, j, visitted);
}

void reduceToRelevant(int *flowRates, int *costs, int *relFlowRates, int *relCosts, int start, int *costsFromStart)
{
    int indices[REL_VALVES];
    {
        int j = 0;
        for (int i = 0; i < VALVES; i++)
            if (flowRates[i])
            {
                indices[j] = i;
                relFlowRates[j++] = flowRates[i];
            }
    }
    for (int i = 0; i < REL_VALVES; i++)
        for (int j = 0; j < REL_VALVES; j++)
            relCosts[__(i,j)] = costs[_(indices[i],indices[j])];
    for (int i = 0; i < REL_VALVES; i++)
        costsFromStart[i] = costs[_(start, indices[i])];
}

void getRelevantGraph(FILE *in, int *relFlowRates, int *relCosts, int *costsFromStart)
{
    int flowRates[VALVES];
    int costs[VALVES*VALVES];
    int tunnels[VALVES*VALVES];
    int start = readValves(in, flowRates, tunnels);
    calculateCosts(tunnels, costs);
    reduceToRelevant(flowRates, costs, relFlowRates, relCosts, start, costsFromStart);
}

