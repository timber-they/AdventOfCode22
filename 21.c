#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MONKEYS 2519
//#define MONKEYS 15

typedef struct Monkey
{
    long value;
    char op;
    int indexA;
    int indexB;
    // Only temporary for reading
    int name;
    int nameA;
    int nameB;
    // For part 2
    int containsX;
} Monkey;

long part1(FILE *in);
long part2(FILE *in);
int sNameToName(char *sName);
void read(FILE *in, Monkey *monkeys);
long solve(Monkey *monkeys, int magicName);
void print(Monkey *monkeys, int index, int depth);
int fillContainsX(Monkey *monkeys, int index);
// Contains the value of x (the human monkey), once no reduction is possible anymore
long reduce(Monkey *monkeys, int index);

int main()
{
    FILE *in = fopen("in21", "r");

    printf("Part1: %ld\n", part1(in));
    rewind(in);
    printf("Part2: %ld\n", part2(in));

    fclose(in);
    return 0;
}

long part1(FILE *in)
{
    int magicName = sNameToName("root");
    Monkey monkeys[MONKEYS] = {0};
    read(in, monkeys);
    return solve(monkeys, magicName);
}

long part2(FILE *in)
{
    int magicName = sNameToName("root");
    int humanName = sNameToName("humn");
    Monkey monkeys[MONKEYS] = {0};
    read(in, monkeys);

    int humanIndex;
    for (humanIndex = 0; humanIndex < MONKEYS; humanIndex++)
        if (monkeys[humanIndex].name == humanName)
            break;
    if (humanIndex == MONKEYS)
        fprintf(stderr, "Human not found!\n");
    monkeys[humanIndex].op = 'h';

    int rootIndex;
    for (rootIndex = 0; rootIndex < MONKEYS; rootIndex++)
        if (monkeys[rootIndex].name == magicName)
            break;
    if (rootIndex == MONKEYS)
        fprintf(stderr, "Root not found!\n");
    monkeys[rootIndex].op = '=';

    solve(monkeys, magicName);
    //printf("Original:\n");
    //print(monkeys, rootIndex, 0);
    fillContainsX(monkeys, rootIndex);

    while(1)
    {
        long res = reduce(monkeys, rootIndex);
        if (res)
            return res;
        //printf("Reduced:\n");
        //print(monkeys, rootIndex, 0);
    }

    return -1;
}

int sNameToName(char *sName)
{
    return ((int)sName[0]-26)*26*26*26 +
        ((int)sName[1]-26)*26*26 +
        ((int)sName[2]-26)*26 +
        ((int)sName[3]-26);
}

void read(FILE *in, Monkey *monkeys)
{
    for (int i = 0; i < MONKEYS; i++)
    {
        char sName[6], sNameA[5], sNameB[5];
        char op = '\0';
        int value = 0;
        long tell = ftell(in);
        if (fscanf(in, "%s %d\n", sName, &value) != 2)
        {
            fseek(in, tell, SEEK_SET);
            if (fscanf(in, "%s %s %c %s\n", sName, sNameA, &op, sNameB) != 4)
                fprintf(stderr, "ERROR in read!\n");
        }
        monkeys[i].value = value;
        monkeys[i].op = op;
        monkeys[i].name = sNameToName(sName);
        monkeys[i].nameA = sNameToName(sNameA);
        monkeys[i].nameB = sNameToName(sNameB);
    }

    for (int i = 0; i < MONKEYS; i++)
    {
        if (monkeys[i].op == '\0')
            // Not depending on other monkeys
            continue;
        for (int j = 0; j < MONKEYS; j++)
            if (monkeys[j].name == monkeys[i].nameA)
            {
                monkeys[i].indexA = j;
                break;
            }
        for (int j = 0; j < MONKEYS; j++)
            if (monkeys[j].name == monkeys[i].nameB)
            {
                monkeys[i].indexB = j;
                break;
            }
    }
}

long solve(Monkey *monkeys, int magicName)
{
    while(1)
    {
        int didSomething = 0;
        for (int i = 0; i < MONKEYS; i++)
        {
            if (monkeys[i].name == magicName && monkeys[i].op == '\0')
                return monkeys[i].value;
            if (monkeys[i].op != '\0' &&
                    monkeys[i].op != 'h' &&
                    monkeys[monkeys[i].indexA].op == '\0' &&
                    monkeys[monkeys[i].indexB].op == '\0')
            {
                long a = monkeys[monkeys[i].indexA].value;
                long b = monkeys[monkeys[i].indexB].value;
                switch(monkeys[i].op)
                {
                    case '+':
                        monkeys[i].value = (long)a+b;
                        break;
                    case '-':
                        monkeys[i].value = (long)a-b;
                        break;
                    case '*':
                        monkeys[i].value = (long)a*b;
                        break;
                    case '/':
                        monkeys[i].value = (long)a/b;
                        break;
                    case '=':
                        monkeys[i].value = (a == b);
                }
                monkeys[i].op = '\0';
                didSomething = 1;
            }
        }
        if (!didSomething)
            return -1;
    }
    return -1;
}

void print(Monkey *monkeys, int index, int depth)
{
    if (monkeys[index].op == '\0')
    {
        for (int i = 0; i < depth; i++)
            printf(" ");
        printf("%ld\n", monkeys[index].value);
        return;
    }
    else if (monkeys[index].op == 'h')
    {
        for (int i = 0; i < depth; i++)
            printf(" ");
        printf("x\n");
        return;
    }
    //for (int i = 0; i < depth; i++)
        //printf(" ");
    //printf("(\n");
    print(monkeys, monkeys[index].indexA, depth+1);
    for (int i = 0; i < depth+1; i++)
        printf(" ");
    printf("%c\n", monkeys[index].op);
    print(monkeys, monkeys[index].indexB, depth+1);
    //for (int i = 0; i < depth; i++)
        //printf(" ");
    //printf(")\n");
    if (monkeys[index].op == '=')
        // Root
        printf("\n");
}

int fillContainsX(Monkey *monkeys, int index)
{
    if (monkeys[index].op == 'h')
        return (monkeys[index].containsX = 1);
    else if (monkeys[index].op == '\0')
        return (monkeys[index].containsX = 0);
    else
        return (monkeys[index].containsX =
                (fillContainsX(monkeys, monkeys[index].indexA) ||
                 fillContainsX(monkeys, monkeys[index].indexB)));
}

long reduce(Monkey *monkeys, int index)
{
    int leftIsLeft = monkeys[monkeys[index].indexA].containsX;
    Monkey *rightMonkey = &monkeys[leftIsLeft ? monkeys[index].indexB : monkeys[index].indexA];
    if (rightMonkey->op != '\0')
        fprintf(stderr, "Expected number on 'right' side!\n");
    Monkey *leftMonkey = &monkeys[leftIsLeft ? monkeys[index].indexA : monkeys[index].indexB];
    if (!leftMonkey->containsX)
        fprintf(stderr, "Expected x on 'left' side!\n");
    if (leftMonkey->op == 'h')
        // Already completely reduced!
        return rightMonkey->value;

    int leftIsNumber = monkeys[leftMonkey->indexA].op == '\0';
    Monkey *leftNumberMonkey = &monkeys[leftIsNumber ? leftMonkey->indexA : leftMonkey->indexB];
    if (leftNumberMonkey->op != '\0')
        fprintf(stderr, "Expected number monkey on 'left' side!\n");
    int leftOperationIndex = leftIsNumber ? leftMonkey->indexB : leftMonkey->indexA;

    switch(leftMonkey->op)
    {
        case '+':
            rightMonkey->value -= leftNumberMonkey->value;
            if (leftIsLeft)
                monkeys[index].indexA = leftOperationIndex;
            else
                monkeys[index].indexB = leftOperationIndex;
            break;
        case '-':
            if (leftIsNumber)
                rightMonkey->value = leftNumberMonkey->value - rightMonkey->value;
            else
                rightMonkey->value += leftNumberMonkey->value;
            if (leftIsLeft)
                monkeys[index].indexA = leftOperationIndex;
            else
                monkeys[index].indexB = leftOperationIndex;
            break;
        case '*':
            rightMonkey->value /= leftNumberMonkey->value;
            if (leftIsLeft)
                monkeys[index].indexA = leftOperationIndex;
            else
                monkeys[index].indexB = leftOperationIndex;
            break;
        case '/':
            if (leftIsNumber)
                rightMonkey->value = leftNumberMonkey->value / rightMonkey->value;
            else
                rightMonkey->value *= leftNumberMonkey->value;
            if (leftIsLeft)
                monkeys[index].indexA = leftOperationIndex;
            else
                monkeys[index].indexB = leftOperationIndex;
            break;
    }
    return 0;
}

