#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stats.h"
#include "data.h"

// short hands
static double get(const DataPoint *dp, int col) {
    return *((double*)dp + col);
}

static void set(DataPoint *dp, int col, double with) {
    *((double*)dp + col) = with;
}

// used for qsort
int cmp_dbl(const void *a, const void *b) {
    double x = *(const double*) a;
    double y = *(const double*) b;

    return (x > y) - (x < y); // 1 if more, 0 if same, -1 if less
}

// utils:
double calc_mean(const Dataset* ds, int col) {
    double sum = 0.0;
    int count = 0;
    
    for (int i = 0; i < ds->count; i++) {
        double v = get(&ds->points[i], col);

        if (!isnan(v)) {
            sum += v;
            count++;
        }
    }

    return count > 0 ? sum / count : 0.0;
}

double calc_median(const Dataset* ds, int col) {
    double *vals = malloc(ds->count * sizeof(double));
    int valid = 0;

    for (int i = 0; i < ds->count; i++) {
        double v = get(&ds->points[i], col);
        
        if (!isnan(v)) {
            vals[valid++] = v;
        }
    }

    if (!valid) {
        free(vals);
        return 0.0;
    }

    qsort(vals, valid, sizeof(double), cmp_dbl);
    double median = vals[valid / 2];

    free(vals);
    return median;
}

void calculate_stats(const Dataset *ds, ColumnStats stats[]) {
    for (int col = 0; col < 9; col++) {
        stats[col].mean = calc_mean(ds, col);
        stats[col].median = calc_median(ds, col);
        stats[col].missing = 0;
        stats[col].min = INFINITY;
        stats[col].max = -INFINITY;

        for (int i = 0; i < ds->count; i++) {
            double v = get(&ds->points[i], col);
            if (isnan(v)) {
                stats[col].missing++;
            } else {
                if (v < stats[col].min) stats[col].min = v;
                if (v > stats[col].max) stats[col].max = v;
            }
        }
    }

    printf("Statistics calculated\n");
    printf("Missing values per column:\n");
    const char *names[] = {"longitude", "latitude", "age", "rooms", 
                          "bedrooms", "population", "households", "income", "value"};
    for (int i = 0; i < 9; i++) {
        if (stats[i].missing > 0) {
            printf("  %s: %d missing\n", names[i], stats[i].missing);
        }
    }
}

void impute_missing(Dataset *ds, const ColumnStats stats[]) {
    int total = 0;

    for (int col = 0; col < 9; col++) {
        if (stats[col].missing > 0) {
            for (int i = 0; i < ds->count; i++) {
                if (isnan(get(&ds->points[i], col))) {
                    set(&ds->points[i], col, stats[col].mean);
                    total++;
                }
            }
        }
    }

    if (total > 0) {
        printf("Imputed %d missing values\n", total);
    }
} 