#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define VALVES 51
//#define VALVES 10
#define REL_VALVES 15
//#define REL_VALVES 6
#define TIME 30

#define _(from,to) ((to)*(VALVES) + (from))
#define __(from,to) ((to)*(REL_VALVES) + (from))
#define GET_STATE(states,i) (((states)&(1<<i))>>i)
#define SET_STATE(states,i) ((states) = (states) | (1<<(i)))
#define CLR_STATE(states,i) ((states) = (states) & (~(1<<(i))))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

int part1(FILE *in);
int part2(FILE *in);
int readValves(FILE *in, int *flowRates, int *tunnels);
void calculateCosts(int *tunnels, int *costs);
void reduceToRelevant(int *flowRates, int *costs, int *relFlowRates, int *relCosts, int start, int *costsFromStart);
void getRelevantGraph(FILE *in, int *relFlowRates, int *relCosts, int *costsFromStart);
int calculateKey(int position, int valveStates, int leftTime);
int getMostPressure(int *flowRates, int *costs, int position, int valveStates, int leftTime, int *memory, int visittedSinceChange);

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
    int max = 0;
    int *memory = malloc(((1<<REL_VALVES)*(TIME+1)*REL_VALVES)*sizeof(*memory));
    for (int i = 0; i < (1<<REL_VALVES)*(TIME+1)*REL_VALVES; i++)
        memory[i] = -1;
    for (int i = 0; i < REL_VALVES; i++)
    {
        int score = getMostPressure(flowRates, costs, i, 0, TIME-costsFromStart[i], memory, 0);
        if (score > max)
            max = score;
    }
    free(memory);
    return max;
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

void calculateCosts(int *tunnels, int *costs)
{
    // Floyd
    for (int i = 0; i < VALVES*VALVES; i++)
        costs[i] = 1<<29;
    for (int i = 0; i < VALVES; i++)
        for (int j = 0; j < VALVES; j++)
            if (tunnels[_(i,j)])
                costs[_(i,j)] = 1;
    for (int i = 0; i < VALVES; i++)
        costs[_(i,i)] = 0;
    for (int k = 0; k < VALVES; k++)
        for (int i = 0; i < VALVES; i++)
            for (int j = 0; j < VALVES; j++)
                costs[_(i,j)] = MIN(costs[_(i,k)]+costs[_(k,j)], costs[_(i,j)]);
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
    int flowRates[VALVES] = {0};
    int costs[VALVES*VALVES] = {0};
    int tunnels[VALVES*VALVES] = {0};
    int start = readValves(in, flowRates, tunnels);
    calculateCosts(tunnels, costs);
    reduceToRelevant(flowRates, costs, relFlowRates, relCosts, start, costsFromStart);
}

int calculateKey(int position, int valveStates, int leftTime)
{
    // position: 0-14, valveStates: 0-2^15-1, leftTime: 0-30
    return position*(1<<REL_VALVES)*(TIME+1)
        + leftTime*(1<<REL_VALVES)
        + valveStates;
}

int getMostPressure(int *flowRates, int *costs, int position, int valveStates, int leftTime, int *memory, int visittedSinceChange)
{
    if (leftTime <= 0)
        return 0;
    int key = calculateKey(position, valveStates, leftTime);
    if (memory[key] >= 0)
        return memory[key];
    int max = 0;
    if (!GET_STATE(valveStates,position))
    {
        SET_STATE(valveStates,position);
        max = getMostPressure(flowRates, costs, position, valveStates, leftTime-1, memory, 0) + flowRates[position]*(leftTime-1);
        CLR_STATE(valveStates,position);
    }
    SET_STATE(visittedSinceChange, position);
    for (int i = 0; i < REL_VALVES; i++)
    {
        // No need to visit already open valves
        if (GET_STATE(valveStates, i) || GET_STATE(visittedSinceChange, i))
            continue;
        int score = getMostPressure(flowRates, costs, i, valveStates, leftTime-costs[__(position,i)], memory, visittedSinceChange);
        if (score > max)
            max = score;
    }
    CLR_STATE(visittedSinceChange, position);
    return (memory[key] = max);
}

