#include <stdlib.h>
#include "scaler.h"

void scale_matrix(Matrix *x, MatScaler *scaler) {
    scaler->size = x->cols;
    scaler->median = malloc(x->cols * sizeof(double));
    scaler->iqr = malloc(x->cols * sizeof(double));
    
    for (int col = 0; col < x->cols; col++) {
        double *column = malloc(x->rows * sizeof(double));
        for (int row = 0; row < x->rows; row++) {
            column[row] = x->data[row][col];
        }
        
        qsort(column, x->rows, sizeof(double), cmp_dbl);
        
        // median = q2
        scaler->median[col] = column[x->rows / 2];
        
        int q1_index = (int)(0.25 * x->rows);
        int q3_index = (int)(0.75 * x->rows);
        double q1 = column[q1_index];
        double q3 = column[q3_index];
        scaler->iqr[col] = q3 - q1;
        
        // robust scaling: (x - median) / IQR
        for (int row = 0; row < x->rows; row++) {
            if (scaler->iqr[col] > 0) {
                x->data[row][col] = (x->data[row][col] - scaler->median[col]) / scaler->iqr[col];
            } else {
                x->data[row][col] = 0.0;
            }
        }
        
        free(column);
    }
}

// void scale_matrix(Matrix *x, MatScaler *scaler) {
//     scaler->size = x->cols;
//     scaler->median = malloc(x->cols * sizeof(double)); // reuse for mean
//     scaler->iqr = malloc(x->cols * sizeof(double));

//     for (int col = 0; col < x->cols; col++) {
//         double sum = 0.0;
//         for (int row = 0; row < x->rows; row++) {
//             sum += x->data[row][col];
//         }
//         double mean = sum / x->rows;
//         scaler->median[col] = mean; // store mean here

//         double sq_sum = 0.0;
//         for (int row = 0; row < x->rows; row++) {
//             double diff = x->data[row][col] - mean;
//             sq_sum += diff * diff;
//         }
//         double std = sqrt(sq_sum / x->rows);
//         if (std == 0.0) std = 1.0; // avoid division by zero
//         scaler->iqr[col] = std; // std is saved as iqr

//         // scale
//         for (int row = 0; row < x->rows; row++) {
//             x->data[row][col] = (x->data[row][col] - mean) / std;
//         }
//     }
// }