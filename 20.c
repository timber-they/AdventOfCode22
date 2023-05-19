#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define NUMBERS 5000
//#define NUMBERS 7

#define _(x) (((x) < 0 ? (x)+((NUMBERS)*(((-(x))%NUMBERS)+1)) : (x)) % NUMBERS)

typedef struct MutArray
{
    // The indices the originally ordered numbers are found at
    // -> Are mutated
    int *indices;
    // The indices of the original numbers in the new order
    // -> Are reordered
    int *valueIndices;
    // The original values in original order
    // -> Are unchanged
    int *values;
} MutArray;

int part1(FILE *in);
int part2(FILE *in);
void read(FILE *in, MutArray arr);
void move(MutArray arr, int index);
void print(MutArray arr);
int mutIndexOfZero(MutArray arr);

int main()
{
    FILE *in = fopen("in20", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    int indices[NUMBERS];
    int valueIndices[NUMBERS];
    int values[NUMBERS];
    MutArray arr = (MutArray) {
        .indices = indices,
        .valueIndices = valueIndices,
        .values = values
    };
    read(in, arr);
    for (int i = 0; i < NUMBERS; i++)
        move(arr, i);
    int zeroIndex = mutIndexOfZero(arr);
    return arr.values[_(arr.valueIndices[_(zeroIndex+1000)])] +
        arr.values[_(arr.valueIndices[_(zeroIndex+2000)])] +
        arr.values[_(arr.valueIndices[_(zeroIndex+3000)])];
}

int part2(FILE *in)
{
    return in == NULL ? -2 : -3;
}

void read(FILE *in, MutArray arr)
{
    for (int i = 0; i < NUMBERS; i++)
    {
        if (fscanf(in, "%d\n", arr.values+i) != 1)
            fprintf(stderr, "ERROR in reading!\n");
        arr.indices[i] = i;
        arr.valueIndices[i] = i;
    }
}

void move(MutArray arr, int index)
{
    int mutIndex = arr.indices[_(index)];
    int move = arr.values[_(index)];
    if (move == 0)
        return;
    int sign = move > 0 ? 1 : -1;
    for (int i = 1; i <= move*sign; i++)
    {
        arr.valueIndices[_(mutIndex+(i*sign)-sign)] =
            arr.valueIndices[_(mutIndex+(i*sign))];
        arr.indices[_(arr.valueIndices[_(mutIndex+(i*sign)-sign)])] -= sign;
    }
    arr.valueIndices[_(mutIndex+move)] = index;
    arr.indices[_(index)] = _(mutIndex+move);
}

void print(MutArray arr)
{
    for (int i = 0; i < NUMBERS; i++)
        printf("%d, ", arr.values[_(arr.valueIndices[_(i)])]);
    printf("\n");
}

int mutIndexOfZero(MutArray arr)
{
    for (int i = 0; i < NUMBERS; i++)
        if (arr.values[_(arr.valueIndices[_(i)])] == 0)
            return i;
    fprintf(stderr, "Zero not found!\n");
    return -1;
}

