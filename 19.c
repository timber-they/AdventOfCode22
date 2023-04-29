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

// 0/0 := 0
#define ROOF_DIVIDE(a,b) ((a) == 0 ? 0 : \
        ((b) == 0 ? (1<<29) : \
        ((a)/(b)+((a)%(b)!=0))))

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
void reward(int *a, int *b, Blueprint blueprint, int *robots, int mul);
int couldPay(int *resources, Blueprint blueprint, int i);
void testRoofDivide();

int main()
{
    testRoofDivide();
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
    //printf("Calculating max geodes with time %d (minute=%d), resources %d,%d,%d,%d and robots %d,%d,%d,%d\n", time, TIME+1-time, resources[0], resources[1], resources[2], resources[3], robots[0], robots[1], robots[2], robots[3]);
    int checksumResources = 0;
    for (int i = 0; i < RES_COUNT; i++)
        checksumResources += (i+1)*resources[i];
    //printf("Left time: %d\n", time);
    // No time left!
    if (time <= 0)
        return resources[LAST];

    // Buy nothing
    int next[RES_COUNT];
    reward(resources, next, blueprint, robots, time);
    int max = next[LAST];

    for (int i = 0; i < RES_COUNT; i++)
    {
        int neededTime = 0;
        for (int j = 0; j < RES_COUNT; j++)
        {
            int neededResources = blueprint.robotCost[i*RES_COUNT+j] - resources[j];
            // If the resource j is needed for the robot i
            if (neededResources > 0
                    // ... and there exists no robot to produce the resource
                    && !robots[j])
                // ... then try next robot
                goto nextRobot;
            else if (neededResources <= 0)
                // No time needed
                continue;
            int thisNeededTime = ROOF_DIVIDE(neededResources, robots[j]);
            //printf("Need %d for robot, producing %d per minute, so I need %d minutes!\n", neededResources, robots[j], thisNeededTime);
            if (thisNeededTime >= time-1)
            {
                //printf("Abort!\n");
                goto nextRobot;
            }
            if (thisNeededTime > neededTime)
                neededTime = thisNeededTime;
        }
        //printf("I want to produce %d. I have the resources %d,%d,%d,%d, I need the resources %d,%d,%d,%d, I produce %d,%d,%d,%d so I need time %d!\n", i, resources[0], resources[1], resources[2], resources[3], blueprint.robotCost[i*RES_COUNT+0], blueprint.robotCost[i*RES_COUNT+1], blueprint.robotCost[i*RES_COUNT+2], blueprint.robotCost[i*RES_COUNT+3], robots[0], robots[1], robots[2], robots[3], neededTime);
        //printf("=> needTime=%d\n", neededTime);
        reward(resources, next, blueprint, robots, neededTime);
        pay(i, next, blueprint);
        for (int j = 0; j < RES_COUNT; j++)
            if (next[j] < 0)
                fprintf(stderr, "Negative resources!\n");
        // Building the robot...
        reward(next, next, blueprint, robots, 1);
        robots[i]++;
        int score = maxGeodes(blueprint, time-neededTime-1, next, robots);
        robots[i]--;
        if (score > max)
            max = score;
nextRobot:;
    }

    int checksumResources2 = 0;
    for (int i = 0; i < RES_COUNT; i++)
        checksumResources2 += (i+1)*resources[i];
    if (checksumResources != checksumResources2)
        fprintf(stderr, "Resources changed!\n");
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

void reward(int *a, int *b, Blueprint blueprint, int *robots, int mul)
{
    for (int i = 0; i < RES_COUNT; i++)
        b[i] = a[i];
    for (int i = 0; i < RES_COUNT; i++)
        for (int j = 0; j < RES_COUNT; j++)
            b[j] += blueprint.robotRewards[i*RES_COUNT+j]*robots[i] * mul;
}

int couldPay(int *resources, Blueprint blueprint, int i)
{
    for (int j = 0; j < RES_COUNT; j++)
        if (resources[j] < blueprint.robotCost[i*RES_COUNT+j])
            return 0;
    return 1;
}

void testRoofDivide()
{
    int expected = 0;
    for (int a = 0; a < 20; a++)
        for (int b = 0; b < 20; b++)
        {
            if (a == 0)
                expected = 0;
            else if (b == 0)
                expected = 1<<29;
            else if (a <= b)
                expected = 1;
            else if (a % b == 0)
                expected = a / b;
            else
                expected = a / b + 1;
            int actual = ROOF_DIVIDE(a,b);
            if (actual != expected)
                fprintf(stderr, "roof(%d/%d) should be %d, but was %d!\n", a, b, expected, actual);
            if (b*actual < a && b != 0)
                fprintf(stderr, "roof(%d/%d)*%d=%d*%d<%d!\n", a, b, b, actual, b, a);
        }
    printf("Roof divide test done!\n");
}

