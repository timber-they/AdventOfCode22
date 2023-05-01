#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define RES_COUNT 4
#define LAST ((RES_COUNT)-1)
//#define BLP_COUNT 30
#define BLP_COUNT 2
#define TIME 24
#define MAX_ORE ((TIME)*(TIME+1)/2+1)

//#define BLP_COUNT2 3
#define BLP_COUNT2 2
#define TIME2 32

#define MAX_ROBOTS TIME2

// 0/0 := 0
#define ROOF_DIVIDE(a,b) ((a) == 0 ? 0 : \
        ((b) == 0 ? (1<<29) : \
        ((a)/(b)+((a)%(b)!=0))))
#define MIN(a,b,c) ((a) < (b) ? \
        ((a) < (c) ? (a) : (c)) : \
        ((b) < (c) ? (b) : (c)))

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
int maxGeodes(Blueprint blueprint, int time, int *resources, int *robots);
void pay(int i, int *resources, Blueprint blueprint);
void unpay(int i, int *resources, Blueprint blueprint);
void reward(int *resources, Blueprint blueprint, int *robots, int mul);
int couldPay(int *resources, Blueprint blueprint, int i);
// Returns the time that is needed
int minimumTime(int robotIndex, int *resources, int *robots, Blueprint blueprint, int leftTime, int *best, int *bestRobots, int *bestResources);
void print(int *resources, int *robots);

int main()
{
    FILE *in = fopen("test19", "r");

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
        int robots[RES_COUNT] = {0};
        // 1 Ore collecting robot
        robots[0] = 1;
        int geodes = maxGeodes(blueprints[i], TIME, resources, robots);
        int qualityLevel = blueprints[i].id * geodes;
        sum += qualityLevel;
        printf("Quality level for i=%d: %d (geodes=%d)\n", i, qualityLevel, geodes);
    }
    return sum;
}

int part2(FILE *in)
{
    Blueprint blueprints[BLP_COUNT];
    readBlueprints(in, blueprints);
    int product = 1;
    for (int i = 0; i < BLP_COUNT2; i++)
    {
        int resources[RES_COUNT] = {0};
        int robots[RES_COUNT] = {0};
        // 1 Ore collecting robot
        robots[0] = 1;
        int geodes = maxGeodes(blueprints[i], TIME2, resources, robots);
        product *= geodes;
        printf("Max geodes for task two, blueprint %d: %d\n", i, geodes);
    }
    return product;
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

int maxGeodes(Blueprint blueprint, int time, int *resources, int *robots)
{
    int best[(TIME2+1)*RES_COUNT] = {0};
    int bestRobots[((TIME2+1)*RES_COUNT)*RES_COUNT] = {0};
    int bestResources[((TIME2+1)*RES_COUNT)*RES_COUNT] = {0};

    int nextResources[RES_COUNT];
    int nextRobots[RES_COUNT];
    while (1)
    {
        // Aim for geode robots
        memcpy(nextResources, resources, RES_COUNT*sizeof(*resources));
        memcpy(nextRobots, robots, RES_COUNT*sizeof(*resources));
        int neededTime = minimumTime(LAST, nextResources, nextRobots, blueprint, time, best, bestRobots, bestResources);
        printf("Best robot decisions were:\n");
        for (int i = TIME2; i >= 0; i--)
        {
            for (int j = 3; j >= 0; j--)
            {
                if (best[j*(TIME2+1)+i])
                {
                    printf("[%d,%d,%d,%d] [%d,%d,%d,%d] (%d, %d=%d) ", bestRobots[(j*(TIME2+1)+i)*RES_COUNT+0], bestRobots[(j*(TIME2+1)+i)*RES_COUNT+1], bestRobots[(j*(TIME2+1)+i)*RES_COUNT+2], bestRobots[(j*(TIME2+1)+i)*RES_COUNT+3], bestResources[(j*(TIME2+1)+i)*RES_COUNT+0], bestResources[(j*(TIME2+1)+i)*RES_COUNT+1], bestResources[(j*(TIME2+1)+i)*RES_COUNT+2], bestResources[(j*(TIME2+1)+i)*RES_COUNT+3], j, i, TIME+1-i);
                    switch(best[j*(TIME2+1)+i])
                    {
                        case -1:
                            printf("No time\n");
                            break;
                        case 0:
                            printf("Nothing\n");
                            break;
                        case 1:
                            printf("direct\n");
                            break;
                        case 2:
                            printf("below\n");
                            break;
                        case 3:
                            printf("wait\n");
                            break;
                        case 4:
                            printf("Ore\n");
                            break;
                    }
                    best[j*(TIME2+1)+i] = 0;
                }
            }
        }
        printf("\n\n");

        if (neededTime <= time)
        {
            memcpy(resources, nextResources, RES_COUNT*sizeof(*resources));
            memcpy(robots, nextRobots, RES_COUNT*sizeof(*resources));
            time -= neededTime;
        }
        else
        {
            // Can't build any geode robots anymore - don't build anything then
            reward(resources, blueprint, robots, time);
            break;
        }
    }
    return resources[LAST];
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

void reward(int *resources, Blueprint blueprint, int *robots, int mul)
{
    for (int i = 0; i < RES_COUNT; i++)
        for (int j = 0; j < RES_COUNT; j++)
            resources[j] += blueprint.robotRewards[i*RES_COUNT+j]*robots[i] * mul;
}

int couldPay(int *resources, Blueprint blueprint, int i)
{
    for (int j = 0; j < RES_COUNT; j++)
        if (resources[j] < blueprint.robotCost[i*RES_COUNT+j])
            return 0;
    return 1;
}

// Helper
int directlyBuyRobot(int robotIndex, int *resources, int *robots, Blueprint blueprint, int leftTime, int *best, int *bestRobots, int *bestResources)
{
        for (int k = 0; k < RES_COUNT; k++)
        {
            bestRobots[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = robots[k];
            bestResources[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = resources[k];
        }
        pay(robotIndex, resources, blueprint);
        reward(resources, blueprint, robots, 1);
        robots[robotIndex]++;
        best[robotIndex*(TIME2+1)+leftTime] = 1;
        return 1;
}

int waitForRobot(int robotIndex, int *resources, int *robots, Blueprint blueprint, int leftTime)
{
    int hasRobots = 1;
    int neededTime = 0;
    int time = 1<<29;
    for (int i = 0; i < RES_COUNT; i++)
    {
        int cost = blueprint.robotCost[robotIndex*RES_COUNT+i];
        if (!cost)
            continue;
        if (!robots[i])
        {
            hasRobots = 0;
            break;
        }
        int neededResource = cost - resources[i];
        if (neededResource > 0)
        {
            int thisNeededTime = ROOF_DIVIDE(neededResource, robots[i]);
            if (thisNeededTime > neededTime)
                neededTime = thisNeededTime;
        }
    }
    if (hasRobots && neededTime <= leftTime)
    {
        // +1 for constructing the robot
        time = neededTime+1;
        reward(resources, blueprint, robots, time);
        pay(robotIndex, resources, blueprint);
        robots[robotIndex]++;
    }
    return time;
}

// TODO: At below the *bestRobots somehow switch...
int buildRobotBelow(int robotIndex, int *resources, int *robots, Blueprint blueprint, int leftTime, int waitTime, int robotToBuild, int *best, int *bestRobots, int *bestResources)
{
    int time = 1<<29;
    if (robotIndex == 0)
        return time;

    time = minimumTime(robotToBuild, resources, robots, blueprint, leftTime, best, bestRobots, bestResources);
    if (time > waitTime || time >= leftTime)
        return 1<<29;
    return time + minimumTime(robotIndex, resources, robots, blueprint, leftTime-time, best, bestRobots, bestResources);
}

int minimumTime(int robotIndex, int *resources, int *robots, Blueprint blueprint, int leftTime, int *best, int *bestRobots, int *bestResources)
{
    if (leftTime <= 0)
    {
        best[robotIndex*(TIME2+1)+leftTime] = -1;
        for (int k = 0; k < RES_COUNT; k++)
        {
            bestRobots[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = robots[k];
            bestResources[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = resources[k];
        }
        return 1<<29;
    }

    /////////////////////////////
    // I can directly buy one! //
    /////////////////////////////
    if (couldPay(resources, blueprint, robotIndex))
        return directlyBuyRobot(robotIndex, resources, robots, blueprint, leftTime, best, bestRobots, bestResources);

    ///////////////////////
    // Trying by waiting //
    ///////////////////////
    int waitResources[RES_COUNT];
    int waitRobots[RES_COUNT];
    memcpy(waitResources, resources, RES_COUNT*sizeof(*resources));
    memcpy(waitRobots, robots, RES_COUNT*sizeof(*robots));
    int waitTime = waitForRobot(robotIndex, waitResources, waitRobots, blueprint, leftTime);

    //////////////////////////////////// 
    // Trying by building robot below //
    ////////////////////////////////////
    int belowResources[RES_COUNT];
    int belowRobots[RES_COUNT];
    int belowBest[(TIME2+1)*RES_COUNT] = {0};
    int belowBestRobots[((TIME2+1)*RES_COUNT)*RES_COUNT] = {0};
    int belowBestResources[((TIME2+1)*RES_COUNT)*RES_COUNT] = {0};
    memcpy(belowResources, resources, RES_COUNT*sizeof(*resources));
    memcpy(belowRobots, robots, RES_COUNT*sizeof(*robots));
    memcpy(belowBest, best, (TIME2+1)*RES_COUNT*sizeof(*best));
    memcpy(belowBestRobots, best, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    memcpy(belowBestResources, best, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    int belowTime = buildRobotBelow(robotIndex, belowResources, belowRobots, blueprint, leftTime, waitTime, robotIndex-1, belowBest, belowBestRobots, belowBestResources);

    ///////////////////////////////
    // Trying building ore robot //
    ///////////////////////////////
    int oreResources[RES_COUNT];
    int oreRobots[RES_COUNT];
    int oreBest[(TIME2+1)*RES_COUNT] = {0};
    int oreBestRobots[((TIME2+1)*RES_COUNT)*RES_COUNT] = {0};
    int oreBestResources[((TIME2+1)*RES_COUNT)*RES_COUNT] = {0};
    memcpy(oreResources, resources, RES_COUNT*sizeof(*resources));
    memcpy(oreRobots, robots, RES_COUNT*sizeof(*robots));
    memcpy(oreBest, best, (TIME2+1)*RES_COUNT*sizeof(*best));
    memcpy(oreBestRobots, best, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    memcpy(oreBestResources, best, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    int oreTime = buildRobotBelow(robotIndex, oreResources, oreRobots, blueprint, leftTime, waitTime, 0, oreBest, oreBestRobots, oreBestResources);


    //////////////////////
    // Final evaluation //
    //////////////////////
    // TODO: What to do if both is the same? More robots vs. more resources...
    //if (belowTime == waitTime)
       //printf("Not sure what to do...\n");
    int minCompared = MIN(belowTime, waitTime, oreTime);
    if (minCompared >= 1<<29)
    {
        best[robotIndex*(TIME2+1)+leftTime] = -1;
        for (int k = 0; k < RES_COUNT; k++)
        {
            bestRobots[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = robots[k];
            bestResources[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = resources[k];
        }
        return 1<<29;
    }
    else if (belowTime == minCompared)
    {
        memcpy(best, belowBest, (TIME2+1)*RES_COUNT*sizeof(*best));
        memcpy(bestRobots, belowBestRobots, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
        memcpy(bestResources, belowBestResources, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
        best[robotIndex*(TIME2+1)+leftTime] = 2;
        for (int k = 0; k < RES_COUNT; k++)
        {
            bestRobots[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = robots[k];
            bestResources[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = resources[k];
        }
        memcpy(resources, belowResources, RES_COUNT*sizeof(*resources));
        memcpy(robots, belowRobots, RES_COUNT*sizeof(*resources));
        return belowTime;
    }
    else if (oreTime == minCompared)
    {
        memcpy(best, oreBest, (TIME2+1)*RES_COUNT*sizeof(*best));
        memcpy(bestRobots, oreBestRobots, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
        memcpy(bestResources, oreBestResources, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
        best[robotIndex*(TIME2+1)+leftTime] = 4;
        for (int k = 0; k < RES_COUNT; k++)
        {
            bestRobots[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = robots[k];
            bestResources[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = resources[k];
        }
        memcpy(resources, oreResources, RES_COUNT*sizeof(*resources));
        memcpy(robots, oreRobots, RES_COUNT*sizeof(*resources));
        return oreTime;
    }
    else if (waitTime == minCompared)
    {
        // If wait was the best, all belows are invalid
        for (int i = leftTime; i >= 0; i--)
        {
            for (int j = 0; j < RES_COUNT; j++)
                best[j*(TIME2+1)+i] = 0;
        }
        best[robotIndex*(TIME2+1)+leftTime] = 3;
        for (int k = 0; k < RES_COUNT; k++)
        {
            bestRobots[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = robots[k];
            bestResources[(robotIndex*(TIME2+1)+leftTime)*RES_COUNT+k] = resources[k];
        }
        memcpy(resources, waitResources, RES_COUNT*sizeof(*resources));
        memcpy(robots, waitRobots, RES_COUNT*sizeof(*resources));
        return waitTime;
    }
    else
    {
        fprintf(stderr, "No time was min time!\n");
        exit(1);
    }
}

void print(int *resources, int *robots)
{
    printf("[");
    for (int i = 0; i < RES_COUNT-1; i++)
        printf("%d, ", resources[i]);
    printf("%d], [", resources[LAST]);
    for (int i = 0; i < RES_COUNT-1; i++)
        printf("%d, ", robots[i]);
    printf("%d]\n", robots[LAST]);
}

