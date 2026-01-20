#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// internal functions used only on this file:

Dataset* allocate_dataset() {
    Dataset *dataset = malloc(sizeof(Dataset));
    dataset->cap = 1000;
    dataset->count = 0;
    dataset->points = malloc(dataset->cap * sizeof(DataPoint));
    return dataset; 
}

void expand_dataset(Dataset* dataset) {
    dataset->cap *= 2;
    dataset->points = realloc(dataset->points, dataset->cap * sizeof(DataPoint));
}

int is_missing(const char* str) {
    return strlen(str) == 0 || str[0] == '\0' || strcmp(str, "NA") == 0 || strcmp(str, "nan") == 0;
}

// external functions:

Dataset* load_dataset(const char* path) {
    FILE *file = fopen(path, "r");

    if (!file) {
        fprintf(stderr, "error: failed to open %s\n", path);
        return NULL;
    }

    Dataset* dataset = allocate_dataset();
    char line[MAX_LINE_LEN];

    // skip first line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        if (dataset->count >= dataset->cap) {
            expand_dataset(dataset);
        }

        DataPoint *dp = &dataset->points[dataset->count];
        int col = 0;
        char *line_ptr = line;
        char *token;
        
        line[strcspn(line, "\n")] = '\0';
        
        char *save_ptr = NULL;
        while ((token = strsep(&line_ptr, ",")) != NULL && col < 9) {
            if (strlen(token) == 0 || is_missing(token)) {
                *((double*)dp + col) = NAN;
            } else {
                *((double*)dp + col) = atof(token);
            }
            col++;
        }
        
        dataset->count++;
    }

    fclose(file);
    return dataset;
}

void free_dataset(Dataset *dataset) {
    if (dataset) {
        free(dataset->points);
        free(dataset);
    }
}

void prepare_data(const Dataset *dataset, Matrix *x, Vector *y) {
    *x = create_matrix(dataset->count, NUM_FEATURES);
    *y = create_vector(dataset->count);

    for (int i = 0; i < dataset->count; i++) {
        const DataPoint *dp = &dataset->points[i];

        x->data[i][0] = dp->longitude;
        x->data[i][1] = dp->latitude;
        x->data[i][2] = dp->median_house_age;
        x->data[i][3] = dp->total_rooms;
        x->data[i][4] = dp->total_bedrooms;
        x->data[i][5] = dp->population;
        x->data[i][6] = dp->households;
        x->data[i][7] = dp->median_income;
        
        y->data[i] = dp->median_house_value;
    }
}