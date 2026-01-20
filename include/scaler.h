#pragma once

#include "utils.h"

typedef struct {
    double *min;
    double *max;
    int size;
} MatScaler;

void scale_matrix(Matrix* x, MatScaler* scaler);
double scale_value(double value, double min, double max);