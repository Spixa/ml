#pragma once

#define NUM_FEATURES 9

typedef struct {
    double **data;
    int rows;
    int cols;

} Matrix;

typedef struct {
    double *data;
    int size;
} Vector; // 1xsize matrix

// memory management
Matrix create_matrix(int rows, int cols);
Vector create_vector(int size);
void free_matrix(Matrix* matrix);
void free_vector(Vector* vector);

// random
void init_random();
double random_double(double min, double max);