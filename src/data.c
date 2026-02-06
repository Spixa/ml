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

double encode_ocean_proximity(const char* token) {
    char cleaned[50];
    strncpy(cleaned, token, sizeof(cleaned));
    cleaned[sizeof(cleaned)-1] = '\0';
    
    if (cleaned[0] == '"' || cleaned[0] == '\'') {
        memmove(cleaned, cleaned + 1, strlen(cleaned));
    }
    if (cleaned[strlen(cleaned)-1] == '"' || cleaned[strlen(cleaned)-1] == '\'') {
        cleaned[strlen(cleaned)-1] = '\0';
    }
    
    while (cleaned[0] == ' ') {
        memmove(cleaned, cleaned + 1, strlen(cleaned));
    }

    int len = strlen(cleaned);
    while (len > 0 && cleaned[len-1] == ' ') {
        cleaned[len-1] = '\0';
        len--;
    }
    
    // label encoding mappings
    if (strcmp(cleaned, "<1H OCEAN") == 0 || strcmp(cleaned, "\"<1H OCEAN\"") == 0) {
        return 0.2;
    } else if (strcmp(cleaned, "INLAND") == 0 || strcmp(cleaned, "\"INLAND\"") == 0) {
        return 0.4;
    } else if (strcmp(cleaned, "NEAR OCEAN") == 0 || strcmp(cleaned, "\"NEAR OCEAN\"") == 0) {
        return 0.6;
    } else if (strcmp(cleaned, "NEAR BAY") == 0 || strcmp(cleaned, "\"NEAR BAY\"") == 0) {
        return 0.8;
    } else if (strcmp(cleaned, "ISLAND") == 0 || strcmp(cleaned, "\"ISLAND\"") == 0) {
        return 1.0;
    } else {
        return NAN;
    }
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

        while ((token = strsep(&line_ptr, ",")) != NULL && col < 10) {
            if (col < 9) {  // continuous features
                if (strlen(token) == 0 || is_missing(token)) {
                    *((double*)dp + col) = NAN;
                } else {
                    *((double*)dp + col) = atof(token);
                }
            } 
            else if (col == 9) {  // categorical feature
                if (is_missing(token)) {
                    dp->ocean_proximity = NAN;
                } else {
                    dp->ocean_proximity = encode_ocean_proximity(token);
                }
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
    const double MAX_TOTAL_ROOMS = INFINITY;
    const double MAX_POPULATION = INFINITY; 

    int valid_count = 0;
    for (int i = 0; i < dataset->count; i++) {
        const DataPoint *dp = &dataset->points[i];
        if (dp->total_rooms <= MAX_TOTAL_ROOMS && dp->population <= MAX_POPULATION) {
            valid_count++;
        }
    }
    
    *x = create_matrix(valid_count, NUM_FEATURES);
    *y = create_vector(valid_count);

    int idx = 0;
    for (int i = 0; i < dataset->count; i++) {
        const DataPoint *dp = &dataset->points[i];
        
        if (dp->total_rooms > MAX_TOTAL_ROOMS || dp->population > MAX_POPULATION) {
            continue;
        }

        x->data[idx][0] = dp->longitude;
        x->data[idx][1] = dp->latitude;
        x->data[idx][2] = dp->median_house_age;
        x->data[idx][3] = dp->total_rooms;
        x->data[idx][4] = dp->total_bedrooms;
        x->data[idx][5] = dp->population;
        x->data[idx][6] = dp->households;
        x->data[idx][7] = dp->median_income;
        x->data[idx][8] = dp->ocean_proximity;
        
        y->data[idx] = dp->median_house_value;
        idx++;
    }

    printf("Removed %d outlier rows\n", dataset->count - valid_count);
}