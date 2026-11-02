#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define INPUT_FILE "tests/data.csv"
#define STR(x) #x
#define XSTR(x) STR(x)
#define COL(c) "\033[" XSTR(c) "m"
#define MIN(a, b) (((a) < (b)) ? (a) : (b));
#define MAX(a, b) (((a) > (b)) ? (a) : (b));

#define RED 31
#define MAGENTA 35
#define GREEN 32
#define CYAN 36
#define YELLOW 33
#define BLUE 34
#define RESET 0

typedef struct Record
{
    uint64_t ms;
    double data;
    char label[16];
    char sensor_id[8];
} Record_t;

typedef struct Accumulate
{
    char sensor_id[8];
    char unit[8];

    double max;
    double min;
    double sum;

    uint32_t count;
} Accumulate_t;

void parseFile(Record_t *records);
bool parseRecord(char *line, Record_t *record);
void tokenizeLine(char *line, char *tokens[4]);
void aggregateRecords(Record_t *records, Accumulate_t accumulates[10]);
int sensorToIdx(const char *sensor);
void sensorUnit(char *unit, int idx);

void printAccumulates(Accumulate_t a[10]);

#endif