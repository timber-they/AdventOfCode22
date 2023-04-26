#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define ITERATIONS 2022
#define ITERATIONS2 1000000000000l
#define SIZE 4
#define WIDTH 7
#define JETS 10091
//#define JETS 40
#define PATTERNS 5
#define OFFSET_X 2
#define OFFSET_Y 4
#define REPEATING 6
//#define REPEATING 5

#define _(x,y) ((x)+(WIDTH)*(y))
#define BMG(i,x,y) ((patterns)[(x)+(y)*(SIZE)+(i)*(SIZE)*(SIZE)])
#define MAX(a,b) ((a) > (b) ? (a) : (b))

int patterns[] =
{
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    ///////////
    0, 1, 0, 0,
    1, 1, 1, 0,
    0, 1, 0, 0,
    0, 0, 0, 0,
    ///////////
    0, 0, 1, 0,
    0, 0, 1, 0,
    1, 1, 1, 0,
    0, 0, 0, 0,
    ///////////
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 0, 0, 0,
    ///////////
    1, 1, 0, 0,
    1, 1, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

int patternHeights[] = {0, 2, 2, 3, 1};

int part1(FILE *in);
long part2(FILE *in);
int collides(int part, int *map, int x, int y, int mapSize);
void draw(int part, int *map, int x, int y);
void readJets(FILE *in, int *jets);
// Returns height
int drop(int part, int *map, int *jets, int *jetIndex, int height, int mapSize);
void print(int *map, int height, int mapSize);

int main()
{
    FILE *in = fopen("in17", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %ld\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int jets[JETS];
    readJets(in, jets);
    int height = 0;
    int jetIndex[1] = {0};
    int mapSize = ITERATIONS*SIZE;
    int *map = calloc(WIDTH*mapSize, sizeof(*map));
    for (int i = 0; i < ITERATIONS; i++)
        height = drop(i%PATTERNS, map, jets, jetIndex, height, mapSize);
    free(map);
    return height;
}

long part2(FILE *in)
{
    // Init
    int jets[JETS];
    readJets(in, jets);
    // Measure prequel
    int height = 0;
    int jetIndex[1] = {0};
    int iterations = 100000;
    int mapSize = iterations*SIZE;
    int *map = calloc(WIDTH*mapSize, sizeof(*map));
    int i;
    for (i = 0; i < iterations && *jetIndex != REPEATING; i++)
        height = drop(i%PATTERNS, map, jets, jetIndex, height, mapSize);
    if (i == iterations)
        fprintf(stderr, "Not enough iterations!\n");
    int prequelHeight = height;
    int prequelDropped = i;
    long missingIterations = ITERATIONS2 - prequelDropped;
    //printf("Prequel:\n");
    //print(map, height, mapSize);

    // Measure prequel + one middle (for overlap calculation)
    for (int j = i; j < iterations && (j == i || *jetIndex != REPEATING); j++)
        height = drop(j%PATTERNS, map, jets, jetIndex, height, mapSize);
    int measuredPreMiddleHeight = height;
    free(map);

    // Measure middle
    height = 0;
    map = calloc(WIDTH*mapSize, sizeof(*map));
    for (i = 0; i < iterations && (i == 0 || *jetIndex != REPEATING); i++)
        height = drop(i%PATTERNS, map, jets, jetIndex, height, mapSize);
    //printf("Middle:\n");
    //print(map, height, mapSize);
    free(map);
    if (i == iterations)
        fprintf(stderr, "Not enough iterations!\n");
    int middleHeight = height;
    int middleDropped = i;
    int overlap = prequelHeight + middleHeight - measuredPreMiddleHeight;
    long middleRepetitions = missingIterations/middleDropped;
    missingIterations = missingIterations%middleDropped;

    // Measure sequel
    iterations = missingIterations;
    mapSize = iterations*SIZE;
    height = 0;
    map = calloc(WIDTH*mapSize, sizeof(*map));
    for (i = 0; i < iterations && (i == 0 || *jetIndex != REPEATING); i++)
        height = drop(i%PATTERNS, map, jets, jetIndex, height, mapSize);
    //printf("Sequel:\n");
    //print(map, height, mapSize);
    free(map);
    if (i != iterations)
        fprintf(stderr, "Not enough repetitions (iterations=%d)!\n", iterations);
    int sequelHeight = height;
    //int sequelDropped = i;

    return prequelHeight + (long)middleRepetitions*(middleHeight-overlap) + sequelHeight - overlap;
}

int collides(int part, int *map, int x, int y, int mapSize)
{
    for (int xi = 0; xi < SIZE; xi++)
        for (int yi = 0; yi < SIZE; yi++)
            if (BMG(part, xi, yi) && (x+xi < 0 || x+xi >= WIDTH
                        || y+yi >= mapSize || map[_(x+xi,y+yi)]))
                return 1;
    return 0;
}

void draw(int part, int *map, int x, int y)
{
    for (int xi = 0; xi < SIZE; xi++)
        for (int yi = 0; yi < SIZE; yi++)
            if (BMG(part, xi, yi))
                map[_(x+xi,y+yi)] = 1;
}

void readJets(FILE *in, int *jets)
{
    for (int i = 0; i < JETS; i++)
        jets[i] = fgetc(in) == '>' ? 1 : -1;
}

int drop(int part, int *map, int *jets, int *jetIndex, int height, int mapSize)
{
    int x = OFFSET_X;
    int y = mapSize-height-OFFSET_Y-patternHeights[part];
    while (y < mapSize)
    {
        // Get pushed by jet of gas
        if (!collides(part, map, x+jets[*jetIndex], y, mapSize))
            x += jets[*jetIndex];
        *jetIndex = (*jetIndex+1)%JETS;
        // Fall down
        if (collides(part, map, x, y+1, mapSize))
        {
            draw(part, map, x, y);
            height = MAX(height, mapSize-y);
            //print(map, height, mapSize);
            return height;
        }
        y++;
    }
    fprintf(stderr, "Fell through the floor :(\n");
    return 0;
}

void print(int *map, int height, int mapSize)
{
    for (int y = mapSize-height; y < mapSize; y++)
    {
        for (int x = 0; x < WIDTH; x++)
            if (map[_(x,y)])
                printf("#");
            else
                printf(".");
        printf("\n");
    }
    printf("\n");
}

