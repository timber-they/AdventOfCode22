#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define SENSORS 14//26

int part1(FILE *in);
int part2(FILE *in);
void read(FILE *in, int *sX, int *sY, int *bX, int *bY);
void fillDists(int *sX, int *sY, int *bX, int *bY, int *dists);
void fillRanges(int *sX, int *sY, int *dists, int y, int *starts, int *ends);
int abs(int x);
int countInRow(int *sX, int *sY, int *dists, int row);
// starts and ends should have size count*(count+1)/2
int distinctRanges(int *starts, int *ends, int count);
int makeDistinct(int *start, int *end, int oStart, int oEnd, int *nStart, int *nEnd);

int main()
{
    FILE *in = fopen("test15", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int sX[SENSORS], sY[SENSORS], bX[SENSORS], bY[SENSORS];
    read(in, sX, sY, bX, bY);
    int dists[SENSORS];
    fillDists(sX, sY, bX, bY, dists);
    return countInRow(sX, sY, dists, 10);//2000000);
}

int part2(FILE *in)
{
    int starts[10], ends[10];
    starts[0] = 0;
    starts[1] = -2;
    starts[2] = 7;
    starts[3] = 1;
    ends[0] = 10;
    ends[1] = -1;
    ends[2] = 12;
    ends[3] = 4;
    int count = distinctRanges(starts, ends, 4);
    for (int i = 0; i < count; i++)
        printf("%d-%d\n", starts[i], ends[i]);
    return in == NULL ? -3 : -2;
}

void read(FILE *in, int *sX, int *sY, int *bX, int *bY)
{
    int i = 0;
    while (fscanf(in, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d\n", sX+i, sY+i, bX+i, bY+i) == 4)
        i++;
}

void fillDists(int *sX, int *sY, int *bX, int *bY, int *dists)
{
    for (int i = 0; i < SENSORS; i++)
        dists[i] = abs(sX[i]-bX[i]) + abs(sY[i]-bY[i]);
}

int abs(int x)
{
    return x < 0 ? -x : x;
}

int countInRow(int *sX, int *sY, int *dists, int row)
{
    int sum = 0;
    int starts[SENSORS*(SENSORS+1)/2], ends[SENSORS*(SENSORS+1)/2];
    fillRanges(sX, sY, dists, row, starts, ends);
    int count = distinctRanges(starts, ends, SENSORS);
    printf("Count=%d\n", count);
    for (int i = 0; i < count; i++)
        sum += ends[i]-starts[i]+1;
    return sum;
}

void fillRanges(int *sX, int *sY, int *dists, int y, int *starts, int *ends)
{
    for (int i = 0; i < SENSORS; i++)
    {
        int actualDistance = dists[i] - abs(sY[i] - y);
        starts[i] = sX[i] - actualDistance;
        ends[i] = sX[i] + actualDistance;
    }
}

int distinctRanges(int *starts, int *ends, int count)
{
    int notDistinct = 1;
    while (notDistinct)
    {
        printf("Got %d ranges\n", count);
        notDistinct = 0;
        int newCount = count;
        for (int i = 0; i < count-1; i++)
            for (int j = i+1; j < count; j++)
            {
                int new = makeDistinct(starts+i, ends+i, starts[j], ends[j], starts+newCount, ends+newCount);
                if (new >= 0)
                    notDistinct = 1;
                if (new == 1)
                    // Nothing to do
                    continue;
                if (new == 2)
                    // Got one new interval
                    newCount++;
            }
        // Remove empty intervals
        for (int i = 0; i < count; i++)
            if (starts[i] > ends[i])
            {
                for (int j = i; j < newCount-1; j++)
                {
                    starts[j] = starts[j+1];
                    ends[j] = ends[j+1];
                }
                newCount--;
            }
        count = newCount;
    }
    return count;
}

int makeDistinct(int *start, int *end, int oStart, int oEnd, int *nStart, int *nEnd)
{
    if (oStart > oEnd || *start > *end)
        // Empty interval
        return -1;
    if (oStart > *end || oEnd < *start)
        // No intersection
        return -1;

    int leftStart, leftEnd, rightStart, rightEnd;
    int left = 0, right = 0;

    if (oStart > *start)
    {
        leftStart = *start;
        leftEnd = oStart-1;
        left = leftEnd >= leftStart;
    }
    if (oEnd < *end)
    {
        rightStart = oEnd+1;
        rightEnd = *end;
        right = rightEnd >= rightStart;
    }

    if (left && right)
    {
        *start = leftStart;
        *end = leftEnd;
        *nStart = rightStart;
        *nEnd = rightEnd;
        return 2;
    }
    if (left)
    {
        *start = leftStart;
        *end = leftEnd;
        return 1;
    }
    if (right)
    {
        *start = rightStart;
        *end = rightEnd;
        return 1;
    }
    *start = 0;
    *end = -1;
    return 0;
}

