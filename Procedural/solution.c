#include "solution.h"

void tokenizeLine(char *line, char *tokens[4])
{
    char *token = strtok(line, ",");
    int i = 0;

    while (token != NULL)
    {
        if (i >= 4)
            break;
        tokens[i++] = token;

        token = strtok(NULL, ",");
    }

    tokens[3][strcspn(tokens[3], "\r\n")] = '\0';
}

void aggregateRecords(Record_t *records, Accumulate_t accumulates[10])
{
    const int records_count = 1000;

    for (int i = 0; i < records_count; i++)
    {
        int idx = sensorToIdx(records[i].sensor_id);

        strcpy(accumulates[idx].sensor_id, records[i].sensor_id);
        sensorUnit(accumulates[idx].unit, idx);

        accumulates[idx].max = MAX(accumulates[idx].max, records[i].data);
        if (accumulates[idx].count == 0)
            accumulates[idx].min = records[i].data;
        else
            accumulates[idx].min = MIN(accumulates[idx].min, records[i].data);

        accumulates[idx].sum += records[i].data;
        accumulates[idx].count++;
    }

    // for (int i = 0; i < 10; i++)
    // {
    //      printf(COL(CYAN) "%s  : " COL(BLUE) " %.2f [%s]\n", accumulates[i].sensor_id, accumulates[i].sum / (double)accumulates[i].count, accumulates[i].unit);
    // }
}

bool parseRecord(char *line, Record_t *record)
{
    char *tokens[4];
    tokenizeLine(line, tokens);

    char *end;
    record->ms = strtoull(tokens[0], &end, 0);
    record->data = strtod(tokens[1], &end);
    strncpy(record->label, tokens[2], sizeof(record->label));
    strncpy(record->sensor_id, tokens[3], sizeof(record->sensor_id));

    if (end != NULL)
        return false;
    else
        return true;
}

void parseFile(Record_t *records)
{
    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == 0)
    {
        fprintf(stderr, COL(RED) "\tCould not open file!" INPUT_FILE COL(RESET) "\n");
        exit(1);
    }

    printf(COL(GREEN) "\tOpened file \'" INPUT_FILE COL(BLUE) "\'\n");

    char line[256];
    fgets(line, sizeof(line), fp); // Skip first line as field names

    int record_count = 0;
    while (fgets(line, sizeof(line), fp))
    {
        Record_t record;
        parseRecord(line, &record);

        // printf("[%llu, %.2f, \"%s\", \"%s\"]\n", record.ms, record.data, record.label, record.sensor_id);

        records[record_count++] = record;
    }

    fclose(fp);
    printf(COL(GREEN) "\n\tClosed file \'" INPUT_FILE COL(BLUE) "\'\n");
}

void sensorUnit(char *unit, int idx)
{
    switch (idx)
    {
    case 0: // CO2
        strcpy(unit, "ppm");
        break;
    case 1: // CURRENT
        strcpy(unit, "A");
        break;
    case 2: // HUMID
        strcpy(unit, "%");
        break;
    case 3: // LIGHT
        strcpy(unit, "lux");
        break;
    case 4: // PRESS
        strcpy(unit, "hPa");
        break;
    case 5: // SOIL
        strcpy(unit, "%");
        break;
    case 6: // SOUND
        strcpy(unit, "dB");
        break;
    case 7: // TEMP
        strcpy(unit, "°C");
        break;
    case 8: // VIB
        strcpy(unit, "mm/s");
        break;
    case 9: // VOLT
        strcpy(unit, "V");
        break;
    }
}

int sensorToIdx(const char *sensor)
{
    if (strcmp(sensor, "CO2") == 0)
        return 0;
    else if (strcmp(sensor, "CURR") == 0)
        return 1;
    else if (strcmp(sensor, "HUMID") == 0)
        return 2;
    else if (strcmp(sensor, "LIGHT") == 0)
        return 3;
    else if (strcmp(sensor, "PRESS") == 0)
        return 4;
    else if (strcmp(sensor, "SOIL") == 0)
        return 5;
    else if (strcmp(sensor, "SOUND") == 0)
        return 6;
    else if (strcmp(sensor, "TEMP") == 0)
        return 7;
    else if (strcmp(sensor, "VIB") == 0)
        return 8;
    else if (strcmp(sensor, "VOLT") == 0)
        return 9;
    else
    {
        fprintf(stderr, COL(RED) "Unknown sensor ID : \'%s\' - %u\n", sensor, (uint8_t)sensor[0]);
        return -1;
    }
}

void printAccumulates(Accumulate_t a[10])
{
    printf(COL(RESET) "===========================================\n");
    printf(COL(MAGENTA) "           SENSOR DATA AGGREGATE          \n");
    printf(COL(RESET) "===========================================\n\n");

    for (int i = 0; i < 10; i++)
    {
        printf(COL(MAGENTA) "Sensor : %s                   Unit : %s\n\n", a[i].sensor_id, a[i].unit);
        printf(COL(RED) "MAX :______________________________%.3f\n", a[i].max);
        printf(COL(GREEN) "AVG :_________________%.3f\n", a[i].sum / (double)a[i].count);
        printf(COL(BLUE) "MIN :_______%.3f\n\n", a[i].min);

        printf(COL(RESET) "===========================================\n\n");
    }

    printf(COL(CYAN) "SAMPLES EXAMINED\n" COL(RESET));
    for (int i = 0; i < 10; i++)
    {
        printf(COL(YELLOW) "%8s  :   " COL(RESET) "[=========100%%=======>] " COL(GREEN) "%d / 100 finished\n", a[i].sensor_id, a[i].count);
    }

    printf(COL(RESET) "\n\n============================================\n\n");
}