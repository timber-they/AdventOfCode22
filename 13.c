#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// Guessed
#define MAX_VALUES 100

typedef struct Value
{
    // If this is non-negative, this was actually an integer
    int Integer;
    int Count;
    struct Value *Values;
    int Nothing;
    int IsDivider;
} Value;

int part1(FILE *in);
int part2(FILE *in);
Value readValue(FILE *in);
void freeValue(Value value);
void printValue(Value value);
// Returns 1 if more is available
int readPair(FILE *in, Value *left, Value *right);
int compare(Value *left, Value *right);
Value *listify(Value *value);
Value generateDivider(int n);
void sort(Value *values, int count);

int ValueCount = 0;

int main()
{
    FILE *in = fopen("in13", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

    fclose(in);
    return 0;
}

int part1(FILE *in)
{
    Value left, right;
    int sum = 0;
    int more = 1;
    for (int i = 1; more; i++)
    {
        more = readPair(in, &left, &right);
        if (compare(&left, &right) > 0)
            sum += i;
        freeValue(left);
        freeValue(right);
        ValueCount+=2;
    }
    return sum;
}

int part2(FILE *in)
{
    Value *values = calloc((ValueCount+2), sizeof(*values));
    for (int i = 0; i < ValueCount-1; i+=2)
        readPair(in, values+i, values+i+1);
    values[ValueCount] = generateDivider(2);
    values[ValueCount+1] = generateDivider(6);
    sort(values, ValueCount+2);
    int res = 1;
    for (int i = 0; i < ValueCount+2; i++)
    {
        if (values[i].IsDivider)
            res *= i+1;
        freeValue(values[i]);
    }
    free(values);
    return res;
}

Value readValue(FILE *in)
{
    Value res = {0};
    res.Nothing = 1;
    res.Values = calloc(MAX_VALUES, sizeof(*res.Values));
    res.Integer = -1;
    for (int c = fgetc(in); c != ']'; c = fgetc(in))
    {
        res.Nothing = 0;
        switch(c)
        {
            case '[':
            case ',':
                if (res.Integer >= 0)
                {
                    if (fseek(in, -1, SEEK_CUR))
                    {
                        fprintf(stderr, "ERROR - Seek failed\n");
                        exit(1);
                    }
                    return res;
                }
                res.Values[res.Count++] = readValue(in);
                break;
            default:
                if (c < '0' || c > '9')
                {
                    fprintf(stderr, "ERROR - Got char %c, expecting number\n", c);
                    exit(2);
                }
                if (res.Integer < 0)
                    res.Integer = c-'0';
                else
                    res.Integer = 10*res.Integer + c-'0';
                break;
        }
    }
    if (res.Count == 0 && fseek(in, -1, SEEK_CUR))
    {
        fprintf(stderr, "ERROR - Seek failed\n");
        exit(1);
    }
    return res;
}

void printValue(Value value)
{
    if (value.Nothing)
        return;
    if (value.Integer >= 0)
    {
        printf("%d", value.Integer);
        return;
    }
    printf("[");
    for (int i = 0; i < value.Count; i++)
    {
        printValue(value.Values[i]);
        if (i < value.Count-1)
            printf(",");
    }
    printf("]");
}

void freeValue(Value value)
{
    for (int i = 0; i < value.Count; i++)
        freeValue(value.Values[i]);
    free(value.Values);
}

int readPair(FILE *in, Value *left, Value *right)
{
    *left = readValue(in);
    if (fgetc(in) != '\n')
    {
        fprintf(stderr, "Expected line break after value!\n");
        exit(3);
    }
    *right = readValue(in);
    if (fgetc(in) != '\n')
    {
        fprintf(stderr, "Expected line break after pair!\n");
        exit(3);
    }
    int c = fgetc(in);
    return c == '\n';
}

int compare(Value *left, Value *right)
{
    if (left->Integer >= 0 && right->Integer >= 0)
        return right->Integer - left->Integer;
    if (left->Integer >= 0)
        return compare(listify(left), right);
    if (right->Integer >= 0)
        return compare(left, listify(right));
    for (int i = 0; i < left->Count && i < right->Count; i++)
    {
        int c = compare(left->Values+i, right->Values+i);
        if (c != 0)
            return c;
    }
    return right->Count - left->Count;
}

Value *listify(Value *value)
{
    if (value->Count > 0)
        // Already listified
        return value;
    Value dummy = {0};
    dummy.Integer = value->Integer;
    value->Integer = -2;
    if (value->Values == NULL)
        value->Values = calloc(1, sizeof(*value->Values));
    value->Values[value->Count++] = dummy;
    return value;
}

Value generateDivider(int n)
{
    Value res = {0};
    res.Integer = -1;
    res.Count = 1;
    res.Values = calloc(1, sizeof(*res.Values));
    res.Nothing = 0;
    res.IsDivider = 1;

    res.Values[0].Integer = -1;
    res.Values[0].Count = 1;
    res.Values[0].Values = calloc(1, sizeof(*res.Values));
    res.Values[0].Nothing = 0;
    res.Values[0].IsDivider = 1;

    res.Values[0].Values[0] = (Value) {0};
    res.Values[0].Values[0].Integer = n;
    res.Values[0].Values[0].Nothing = 0;
    res.Values[0].Values[0].IsDivider = 1;

    return res;
}

void sort(Value *values, int count)
{
    int swapped = 1;
    while (swapped)
    {
        swapped = 0;
        for (int i = 0; i < count-1; i++)
        {
            if (compare(values+i, values+i+1) < 0)
            {
                swapped++;
                Value buff = values[i+1];
                values[i+1] = values[i];
                values[i] = buff;
            }
        }
    }
}

