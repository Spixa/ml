#include <math.h>
#include <stdlib.h>
#include "scaler.h"

void scale_matrix(Matrix *x, MatScaler *scaler) {
    scaler->size = x->cols;
    scaler->min = malloc(x->cols * sizeof(double));
    scaler->max = malloc(x->cols * sizeof(double));

    for (int col = 0; col < x->cols; col++) {
        scaler->min[col] = INFINITY;
        scaler->max[col] = -INFINITY;

        for (int row = 0; row < x->rows; row++) {
            double value = x->data[row][col];
            if (value < scaler->min[col]) scaler->min[col] = value;
            if (value > scaler->max[col]) scaler->max[col] = value;
        }

        for (int row = 0; row < x->rows; row++) {
            x->data[row][col] = scale_value(x->data[row][col], scaler->min[col], scaler->max[col]);
        }
    }
}

double scale_value(double value, double min, double max) {
    if (max - min == 0) return 0.0;

    return (value - min) / (max - min);
}