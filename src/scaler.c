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