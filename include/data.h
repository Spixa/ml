#pragma once

#include "utils.h"

#define MAX_LINE_LEN 1024

typedef struct {
    double longitude;
    double latitude;
    double median_house_age;
    double total_rooms;
    double total_bedrooms;
    double population;
    double households;
    double median_income;
    double median_house_value;
} DataPoint;

typedef struct {
    DataPoint *points;
    int count;
    int cap;
} Dataset;

Dataset* load_dataset(const char *path);
void free_dataset(Dataset* dataset);
void prepare_data(const Dataset *dataset, Matrix *x, Vector *y);

Dataset* allocate_dataset();
void expand_dataset(Dataset* dataset);
int is_missing(const char* str);