#include <time.h>
#include <stdlib.h>

#include "utils.h"

Matrix create_matrix(int rows, int cols) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    // init rows
    mat.data = malloc(rows * sizeof(double*));

    // init cols for each row:
    for (int i = 0; i < rows; i++) {
        mat.data[i] = malloc(cols * sizeof(double));
    }

    return mat;
}

Vector create_vector(int size) {
    Vector vec;
    vec.size = size;
    vec.data = malloc(size * sizeof(double));

    return vec;
}

void free_matrix(Matrix *matrix) {
    if (!matrix || !matrix->data) return;

    for (int i = 0; i < matrix->rows; i++) {
        free(matrix->data[i]);
    }

    free(matrix->data);
}

void free_vector(Vector *vector) {
    if (!vector || !vector->data) return;

    free(vector->data);
}

void init_random() {
    srand(time(NULL));
}

double random_double(double min, double max) {
    return min + (rand() / (double)RAND_MAX) * (max - min);
}