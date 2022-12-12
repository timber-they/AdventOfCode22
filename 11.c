#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MONKEYS 8
// Guess
#define THINGS 64

typedef enum
{
    oldTimesOld,
    oldTimesNumber,
    oldPlusOld,
    oldPlusNumber
} Operation;

typedef struct
{
    int items[THINGS];
    int itemCount;
    Operation op;
    int number;
    int test;
    int trueMonkey;
    int falseMonkey;
    int inspections;
} Monkey;

int part1(FILE *in);
int part2(FILE *in);
Monkey *parseMonkeys(FILE *in, Monkey *buff);
void playRound(Monkey *monkeys);
void playMonkey(Monkey *active, Monkey *monkeys);
void inspect(Monkey *monkey, Monkey *monkeys);
int isWorried(Monkey *monkey);
void moveItem(Monkey *from, Monkey *to);
int calculateBusiness(Monkey *monkeys);
void printMonkeys(Monkey *monkeys);

int relief = 1;

int main()
{
    FILE *in = fopen("in11", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    relief = 1;
    Monkey monkeys[MONKEYS] = {0};
    parseMonkeys(in, monkeys);
    for (int i = 0; i < 20; i++)
        playRound(monkeys);
    return calculateBusiness(monkeys);
}

int part2(FILE *in)
{
    relief = 0;
    Monkey monkeys[MONKEYS] = {0};
    parseMonkeys(in, monkeys);
    for (int i = 0; i < 20; i++)
        playRound(monkeys);
    return calculateBusiness(monkeys);
}

Monkey *parseMonkeys(FILE *in, Monkey *buff)
{
    for (int i = 0; i < MONKEYS; i++)
    {
        buff[i].inspections = 0;
        if (fseek(in, strlen("Monkey x:\n  Starting items: "), SEEK_CUR))
        {
            fprintf(stderr, "Seek failed!\n");
            exit(1);
        }

        int curr = 0;
        buff[i].itemCount = 0;
        for (int c = fgetc(in); c != '\n'; c = fgetc(in))
        {
            if (c == ' ')
                continue;
            if (c == ',')
            {
                buff[i].items[buff[i].itemCount++] = curr;
                curr = 0;
                continue;
            }
            curr = curr * 10 + c-'0';
        }
        if (curr != 0)
            buff[i].items[buff[i].itemCount++] = curr;

        char op;
        char s[4] = {0};
        if (fscanf(in, "  Operation: new = old %c %s\n", &op, s) != 2)
        {
            fprintf(stderr, "Scan failed!\n");
            exit(2);
        }
        if (s[0] <= '9' && s[0] >= '0')
        {
            buff[i].number = atoi(s);
            if (op == '*')
                buff[i].op = oldTimesNumber;
            else if (op == '+')
                buff[i].op = oldPlusNumber;
            else
            {
                fprintf(stderr, "Invalid operation: %c!\n", op);
                exit(3);
            }
        }
        else
        {
            buff[i].number = 1;
            if (op == '*')
                buff[i].op = oldTimesOld;
            else if (op == '+')
                buff[i].op = oldPlusOld;
            else
            {
                fprintf(stderr, "Invalid operation: %c!\n", op);
                exit(3);
            }
        }

        if (fscanf(in, "  Test: divisible by %d\n", &buff[i].test) != 1)
        {
            fprintf(stderr, "Scan failed!\n");
            exit(2);
        }

        if (fscanf(in, "    If true: throw to monkey %d\n", &buff[i].trueMonkey) != 1)
        {
            fprintf(stderr, "Scan failed!\n");
            exit(2);
        }

        if (fscanf(in, "    If false: throw to monkey %d\n", &buff[i].falseMonkey) != 1)
        {
            fprintf(stderr, "Scan failed!\n");
            exit(2);
        }
    }
    
    return buff;
}

void playRound(Monkey *monkeys)
{
    for (int i = 0; i < MONKEYS; i++)
        playMonkey(monkeys+i, monkeys);
}

void playMonkey(Monkey *active, Monkey *monkeys)
{
    while (active->itemCount > 0)
    {
        inspect(active, monkeys);
        if (relief)
            active->items[0] /= 3;
        if (isWorried(active))
            moveItem(active, monkeys+active->trueMonkey);
        else
            moveItem(active, monkeys+active->falseMonkey);
    }
}

void inspect(Monkey *monkey, Monkey *monkeys)
{
    if (monkeys == NULL)
        fprintf(stderr, "Where are the monkeys?!\n");
    switch(monkey->op)
    {
        case oldTimesOld:
            monkey->items[0] =
                (monkey->items[0]*monkey->items[0])
                //    % (monkeys[i].test*monkeys[i].number)
                ;
            break;
        case oldTimesNumber:
            monkey->items[0] =
                (monkey->items[0]*monkey->number)
                //    % (monkeys[i].test*monkeys[i].number)
                ;
            break;
        case oldPlusOld:
            monkey->items[0] =
                (monkey->items[0]+monkey->items[0])
                //    % (monkeys[i].test*monkeys[i].number)
                ;
            break;
        case oldPlusNumber:
            monkey->items[0] =
                (monkey->items[0]+monkey->number)
                //    % (monkeys[i].test*monkeys[i].number)
                ;
            break;
    }
    monkey->inspections++;
}

int isWorried(Monkey *monkey)
{
    return monkey->items[0] % monkey->test == 0;
}

void moveItem(Monkey *from, Monkey *to)
{
    to->items[to->itemCount++] = from->items[0];
    from->itemCount--;
    for (int i = 0; i < from->itemCount; i++)
        from->items[i] = from->items[i+1];
}

int calculateBusiness(Monkey *monkeys)
{
    int max = 0,
        second = 0;
    for (int i = 0; i < MONKEYS; i++)
        if (monkeys[i].inspections > max)
        {
            second = max;
            max = monkeys[i].inspections;
        }
        else if (monkeys[i].inspections > second)
            second = monkeys[i].inspections;
    return max*second;
}

void printMonkeys(Monkey *monkeys)
{
    for (int i = 0; i < MONKEYS; i++)
    {
        printf("Items: ");
        for (int j = 0; j < monkeys[i].itemCount; j++)
            printf("%d, ", monkeys[i].items[j]);
        printf("\n");
        printf("Item count: %d\n", monkeys[i].itemCount);
        printf("Operation: %d\n", monkeys[i].op);
        printf("Number: %d\n", monkeys[i].number);
        printf("Test: %d\n", monkeys[i].test);
        printf("True Monkey: %d\n", monkeys[i].trueMonkey);
        printf("False Monkey: %d\n", monkeys[i].falseMonkey);
        printf("Inspections: %d\n", monkeys[i].inspections);
        printf("===========================\n");
    }
}

