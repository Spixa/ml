#pragma once

#include "utils.h"

typedef struct {
    Matrix x_train, x_test;
    Vector y_train, y_test;
    int train_size, test_size;
} DataSplit;

typedef struct {
    double *weights; // A_n's in y = sum{{A_n}{x_n}} + B
    double bias; // B in y = sum{{A_n}{x_n}} + B
    int num_features;
    double cost;
} LinearModel;

void split_data(Matrix *x, Vector *y, double ratio, DataSplit* split);
void free_split(DataSplit *split);

void init_model(LinearModel *model, int num_features);
void free_model(LinearModel *model);

void train(Matrix *x, Vector *y, LinearModel *model, double learning_rate, int max_epochs);
double predict(double* features, LinearModel* model);

void test(Matrix *x_test, Vector *y_test, LinearModel* model);

// demos:
void show_predictions(Matrix *x_test, Vector *y_test, LinearModel* model, int count);
void show_line_equation(LinearModel* model);