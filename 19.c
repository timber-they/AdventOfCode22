#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define RES_COUNT 4
#define LAST ((RES_COUNT)-1)
#define BLP_COUNT 30
//#define BLP_COUNT 2
#define TIME 24
#define MAX_ORE ((TIME)*(TIME+1)/2+1)

#define BLP_COUNT2 3
//#define BLP_COUNT2 2
#define TIME2 32

#define MAX_ORE_ROBOTS 5
#define MAX_CLAY_ROBOTS 15

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
int maxGeodes1(Blueprint blueprint, int time, int *resources, int *dontBuy, int *robots);
int maxGeodes(Blueprint blueprint, int time, int *resources, int *robots);
void pay(int i, int *resources, Blueprint blueprint);
void unpay(int i, int *resources, Blueprint blueprint);
void reward1(int *a, int *b, Blueprint blueprint, int *robots);
void reward(int *resources, Blueprint blueprint, int *robots, int mul);
int couldPay(int *resources, Blueprint blueprint, int i);
// Returns the time that is needed
int minimumTime(int robotIndex, int *resources, int *robots, Blueprint blueprint, int leftTime, int *best, int *bestRobots, int *bestResources);
void print(int *resources, int *robots);

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
        int geodes = maxGeodes1(blueprints[i], TIME, resources, dontBuy, robots);
        int qualityLevel = blueprints[i].id * geodes;
        sum += qualityLevel;
        printf("Quality level for i=%d: %d\n", i, qualityLevel);
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

    // Aim for geode robots
    int geodeResources[RES_COUNT];
    int geodeRobots[RES_COUNT];
    memcpy(geodeResources, resources, RES_COUNT*sizeof(*resources));
    memcpy(geodeRobots, robots, RES_COUNT*sizeof(*resources));
    int neededTimeGeode = minimumTime(LAST, geodeResources, geodeRobots, blueprint, time, best, bestRobots, bestResources);
    // Aim for ore robots
    int oreResources[RES_COUNT];
    int oreRobots[RES_COUNT];
    memcpy(oreResources, resources, RES_COUNT*sizeof(*resources));
    memcpy(oreRobots, robots, RES_COUNT*sizeof(*resources));
    int neededTimeOre = geodeRobots[0] < MAX_ORE_ROBOTS ? minimumTime(0, oreResources, oreRobots, blueprint, time, best, bestRobots, bestResources) : 1<<29;
    // Aim for clay
    int clayResources[RES_COUNT];
    int clayRobots[RES_COUNT];
    memcpy(clayResources, resources, RES_COUNT*sizeof(*resources));
    memcpy(clayRobots, robots, RES_COUNT*sizeof(*resources));
    int neededTimeClay = clayRobots[1] < MAX_CLAY_ROBOTS ? minimumTime(1, clayResources, clayRobots, blueprint, time, best, bestRobots, bestResources) : 1<<29;

    int geodesGeode = 0;
    if (neededTimeGeode <= time)
        geodesGeode = maxGeodes(blueprint, time-neededTimeGeode, geodeResources, geodeRobots);
    else
    {
        // Can't build any geode robots anymore - don't build anything then
        reward(resources, blueprint, robots, time);
        return resources[LAST];
    }
    int geodesOre = 0;
    if (neededTimeOre <= time)
        geodesOre = maxGeodes(blueprint, time-neededTimeOre, oreResources, oreRobots);
    int geodesClay = 0;
    if (neededTimeClay <= time)
        geodesClay = maxGeodes(blueprint, time-neededTimeClay, clayResources, clayRobots);

    if (geodesOre >= geodesGeode && geodesOre >= geodesClay)
    {
        memcpy(resources, oreResources, RES_COUNT*sizeof(*resources));
        memcpy(robots, oreRobots, RES_COUNT*sizeof(*resources));
    }
    else if (geodesGeode >= geodesOre && geodesGeode >= geodesClay)
    {
        memcpy(resources, geodeResources, RES_COUNT*sizeof(*resources));
        memcpy(robots, geodeRobots, RES_COUNT*sizeof(*resources));
    }
    else
    {
        memcpy(resources, clayResources, RES_COUNT*sizeof(*resources));
        memcpy(robots, clayRobots, RES_COUNT*sizeof(*resources));
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
    memcpy(belowBestRobots, bestRobots, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    memcpy(belowBestResources, bestResources, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
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
    memcpy(oreBestRobots, bestRobots, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    memcpy(oreBestResources, bestResources, ((TIME2+1)*RES_COUNT)*RES_COUNT*sizeof(*best));
    int oreTime = buildRobotBelow(robotIndex, oreResources, oreRobots, blueprint, leftTime, waitTime, 0, oreBest, oreBestRobots, oreBestResources);


    //////////////////////
    // Final evaluation //
    //////////////////////
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

void fillPayable(int *resources, Blueprint blueprint, int *payable)
{
    for (int i = 0; i < RES_COUNT; i++)
        payable[i] = couldPay(resources, blueprint, i);
}

int maxGeodes1(Blueprint blueprint, int time, int *resources, int *dontBuy, int *robots)
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
    reward1(resources, next, blueprint, robots);
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
            int score = maxGeodes1(blueprint, time-1, next, empty, robots);
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
    int score = maxGeodes1(blueprint, time-1, next, payable, robots);
    if (score > max)
        max = score;
    return max;
}

void reward1(int *a, int *b, Blueprint blueprint, int *robots)
{
    for (int i = 0; i < RES_COUNT; i++)
        b[i] = a[i];
    for (int i = 0; i < RES_COUNT; i++)
        for (int j = 0; j < RES_COUNT; j++)
            b[j] += blueprint.robotRewards[i*RES_COUNT+j]*robots[i];
}
