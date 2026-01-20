#include "data.h"
#include "stats.h"
#include "utils.h"
#include <stdio.h>

int main() {
    Dataset* ds = load_dataset("housing.csv");

    if (!ds) return 0;
    printf("Loaded %d records.\n", ds->count);

    ColumnStats stats[NUM_FEATURES];
    calculate_stats(ds, stats);

    Matrix x;
    Vector y;
    prepare_data(ds, &x, &y);
}