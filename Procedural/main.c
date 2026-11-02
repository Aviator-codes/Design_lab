#include "solution.h"

int main(int argc, char **argv)
{
    printf("\n\t" COL(MAGENTA) "Hello from procedural example\n");

    Record_t *records = calloc(1000, sizeof(Record_t)); // 1000 records
    parseFile(records);

    Accumulate_t accumulates[10] = {0};
    aggregateRecords(records, accumulates);

    printAccumulates(accumulates);

    printf("\n" COL(RESET));
}
