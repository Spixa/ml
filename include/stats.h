#pragma once

#include "data.h"

typedef struct {
    double mean;
    double median;
    double min;
    double max;
    int missing;
} ColumnStats;

void calculate_stats(const Dataset* ds, ColumnStats stats[]);
void impute_missing(Dataset* ds, const ColumnStats stats[]);

void set(DataPoint *dp, int col, double with);
double get(const DataPoint *dp, int col);