#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define RES_COUNT 4
#define LAST ((RES_COUNT)-1)
#define BLP_COUNT 30
#define TIME 24
#define MAX_ORE ((TIME)*(TIME+1)/2+1)

typedef struct Blueprint
{
    int id;
    int robotRewards[RES_COUNT*RES_COUNT];
    int robotCost[RES_COUNT*RES_COUNT];
} Blueprint;

int part1(FILE *in);
int part2(FILE *in);
Blueprint readBlueprint(FILE *in);
void readBlueprints(FILE *in, Blueprint *blueprints);
int maxGeodes(Blueprint blueprint, int time, int *resources, int *dontBuy, int *robots);
void pay(int i, int *resources, Blueprint blueprint);
void unpay(int i, int *resources, Blueprint blueprint);
void reward(int *a, int *b, Blueprint blueprint, int *robots);
int couldPay(int *resources, Blueprint blueprint, int i);
void fillPayable(int *resources, Blueprint blueprint, int *payable);

int main()
{
    FILE *in = fopen("in19", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    Blueprint blueprints[BLP_COUNT];
    readBlueprints(in, blueprints);
    int sum = 0;
    for (int i = 0; i < BLP_COUNT; i++)
    {
        int resources[RES_COUNT] = {0};
        int dontBuy[RES_COUNT] = {0};
        int robots[RES_COUNT] = {0};
        // 1 Ore collecting robot
        robots[0] = 1;
        int geodes = maxGeodes(blueprints[i], TIME, resources, dontBuy, robots);
        int qualityLevel = blueprints[i].id * geodes;
        sum += qualityLevel;
        printf("Quality level for i=%d: %d\n", i, qualityLevel);
    }
    return sum;
}

int part2(FILE *in)
{
    return in == NULL ? -3 : -2;
}

Blueprint readBlueprint(FILE *in)
{
    int id, oreOre, clayOre, obsidianOre, obsidianClay, geodeOre, geodeObsidian;
    if (fscanf(in, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.\n", &id, &oreOre, &clayOre, &obsidianOre, &obsidianClay, &geodeOre, &geodeObsidian) != 7)
        fprintf(stderr, "Error in reading!\n");
    return (Blueprint)
    {
        .id = id,
        .robotCost =
        {
            oreOre,      0,            0,             0,
            clayOre,     0,            0,             0,
            obsidianOre, obsidianClay, 0,             0,
            geodeOre,    0,            geodeObsidian, 0,
        },
        .robotRewards =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }
    };
}

void readBlueprints(FILE *in, Blueprint *blueprints)
{
    for (int i = 0; i < BLP_COUNT; i++)
        blueprints[i] = readBlueprint(in);
}

int maxGeodes(Blueprint blueprint, int time, int *resources, int *dontBuy, int *robots)
{
    // No time left!
    if (time <= 0)
    {
        //printf("No time left! Current resources: %d,%d,%d,%d\n", resources[0], resources[1], resources[2], resources[3]);
        return resources[LAST];
    }
    // Init stuff
    int max = 0;
    int empty[RES_COUNT] = {0};
    // Get payd
    int next[RES_COUNT] = {0};
    reward(resources, next, blueprint, robots);
    //printf("Reward yielded: %d,%d,%d,%d\n", next[0], next[1], next[2], next[3]);
    // What *can* I pay?
    int payable[RES_COUNT];
    fillPayable(resources, blueprint, payable);
    // Pay random stuff
    for (int i = 0; i < RES_COUNT; i++)
        if (payable[i] && !dontBuy[i])
        {
            pay(i, next, blueprint); 
            robots[i]++;
            int score = maxGeodes(blueprint, time-1, next, empty, robots);
            robots[i]--;
            unpay(i, next, blueprint);
            if (score > max)
            {
                //printf("New max is %d!\n", score);
                max = score;
            }
        }
    // Don't pay anything
    // Set everything I *could* pay to not pay list
    for (int i = 0; i < RES_COUNT; i++)
        payable[i] |= dontBuy[i];
    int score = maxGeodes(blueprint, time-1, next, payable, robots);
    if (score > max)
        max = score;
    return max;
}

void pay(int i, int *resources, Blueprint blueprint)
{
    for (int j = 0; j < RES_COUNT; j++)
        resources[j] = resources[j]-blueprint.robotCost[i*RES_COUNT+j];
}

void unpay(int i, int *resources, Blueprint blueprint)
{
    for (int j = 0; j < RES_COUNT; j++)
        resources[j] = resources[j]+blueprint.robotCost[i*RES_COUNT+j];
}

void reward(int *a, int *b, Blueprint blueprint, int *robots)
{
    for (int i = 0; i < RES_COUNT; i++)
        b[i] = a[i];
    for (int i = 0; i < RES_COUNT; i++)
        for (int j = 0; j < RES_COUNT; j++)
            b[j] += blueprint.robotRewards[i*RES_COUNT+j]*robots[i];
}

int couldPay(int *resources, Blueprint blueprint, int i)
{
    for (int j = 0; j < RES_COUNT; j++)
        if (resources[j] < blueprint.robotCost[i*RES_COUNT+j])
            return 0;
    return 1;
}

void fillPayable(int *resources, Blueprint blueprint, int *payable)
{
    for (int i = 0; i < RES_COUNT; i++)
        payable[i] = couldPay(resources, blueprint, i);
}

