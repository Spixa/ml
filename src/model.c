#include "model.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// internal functions:
static double compute_pred(double *features, LinearModel *model) {
    double result = model->bias; // B in y= Ax + B

    // calculate sum of Ax:
    for (int i = 0; i < model->num_features; i++) {
        result += model->weights[i] * features[i];
    }
    return result;
}

static double compute_mse(Matrix *x, Vector *y, LinearModel *model) {
    double total = 0.0;
    
    // for each entry
    for (int i = 0; i< x->rows; i++) {
        double pred = compute_pred(x->data[i], model);
        double error = pred - y->data[i]; // prediction - actual
        total += error * error; // 
    }

    return total / x->rows; // MSE
}

void split_data(Matrix *x, Vector *y, double ratio, DataSplit *split) {
    init_random();

    int total = x->rows;
    split->train_size = total * ratio;
    split->test_size = total - split->train_size;

    // normal order
    int *indices = malloc(total * sizeof(int));
    for (int i = 0; i < total; i++) indices[i] = i;

    // shuffle
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // swap
        int tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    split->x_train = create_matrix(split->train_size, x->cols);
    split->x_test = create_matrix(split->test_size, x->cols);
    split->y_train = create_vector(split->train_size);
    split->y_test = create_vector(split->test_size);

    for (int i = 0; i < split->train_size; i++) {
        int idx = indices[i];
        split->y_train.data[i] = y->data[idx];
        for (int j = 0; j < x->cols; j++) {
            split->x_train.data[i][j] = x->data[idx][j];
        }
    }

    for (int i = 0; i < split->test_size; i++) {
        int idx = indices[i + split->train_size];
        split->y_test.data[i] = y->data[idx];
        for (int j = 0; j < x->cols; j++) {
            split->x_test.data[i][j] = x->data[idx][j];
        }
    }

    free(indices); // forgot to do this
}

void free_split(DataSplit *split) {
    free_matrix(&split->x_train);
    free_matrix(&split->x_test);
    free_vector(&split->y_train);
    free_vector(&split->y_test);
}

void init_model(LinearModel *model, int num_features) {
    model->num_features = num_features;
    model->weights = malloc(num_features * sizeof(double));
    model->bias = 0.0;
    model->cost = 0.0;
    
    // according to AI we will benefit from initially having random normalized weights rather than zeroed out weights
    init_random();
    for (int i = 0; i < num_features; i++) {
        model->weights[i] = (rand() / (double)RAND_MAX) * 0.01;
    }
}

void free_model(LinearModel *model) {
    if (model->weights) {
        free(model->weights);
    }
}

// actual training code, refactored but initially provided by our TA
void train(Matrix *x, Vector *y, LinearModel *model, double learning_rate, int max_epochs) {
    int m = x->rows;
    int n = model->num_features;

    puts("Training has started");

    for (int epoch = 0; epoch < max_epochs; epoch++) {
        double grad_bias = 0.0;
        double *grad_weights = calloc(n, sizeof(double));

        for (int i = 0; i < m; i++) {
            double pred = compute_pred(x->data[i], model);
            double error = pred - y->data[i];

            grad_bias += error;

            for (int j = 0; j < n; j++) {
                grad_weights[j] += error * x->data[i][j];
            }
        }

        model->bias -= (learning_rate / m) * grad_bias;
        for (int j = 0; j < n; j++) {
            model->weights[j] -= (learning_rate / m) * grad_weights[j];
        }

        free(grad_weights);

        if (epoch % 100 == 0) {
            double mse = compute_mse(x, y, model);
            printf("Epoch %4d: MSE = %.2f\n", epoch, mse);
        }
    }

    model->cost = compute_mse(x, y, model);
}

double predict(double *features, LinearModel *model) {
    return compute_pred(features, model);
}

void test(Matrix *x_test, Vector *y_test, LinearModel *model) {
    int n = x_test->rows;

    if (n == 0) {
        puts("No test data available");
        return;
    }

    double mse = compute_mse(x_test, y_test, model);

    double y_mean = 0.0;
    for (int i = 0; i < n; i++) {
        y_mean += y_test->data[i];
    }
    y_mean /= n;

    double ss_total = 0.0;
    double ss_residual = 0.0;
    for (int i = 0; i < n; i++) {
        double pred = predict(x_test->data[i], model);
        ss_total += pow(y_test->data[i] - y_mean, 2);
        ss_residual += pow(pred - y_test->data[i], 2);
    }

    double r2 = (ss_total > 0) ? 1.0 - (ss_residual / ss_total) : 0.0;

    if (r2 >= 0.7) {
        printf("Good fit: Model explains most price variation\n");
    } else if (r2 >= 0.5) {
        printf("Moderate fit: Model explains some price variation\n");
    } else {
        printf("Poor fit: Consider adding more features or data\n");
    }

    printf("\n=== Test results ===\n");
    printf("Test samples: %d\n", n);
    printf("Mean Squared Error (MSE):  $%.2f\n", mse);
}


void show_predictions(Matrix *x_test, Vector *y_test, LinearModel *model, int count) {
    if (count <= 0 || x_test->rows == 0) return;

    if (count > x_test->rows) count = x_test->rows;

    printf("\n=== Sample Predictions ===\n");
    printf("%-6s %-12s %-12s %-12s %-10s\n", 
        "Index", "Actual", "Predicted", "Error", "Error %");
    printf("%-6s %-12s %-12s %-12s %-10s\n", 
        "------", "------------", "------------", "------------", "----------");
        
    for (int i = 0; i < count; i++) {
        double actual = y_test->data[i];
        double predicted = predict(x_test->data[i], model);
        double error = predicted - actual;
        double error_pct = (fabs(error) / actual) * 100;
        
        printf("%-6d $%-11.0f $%-11.0f $%+-11.0f %-9.1f%%\n", 
            i, actual, predicted, error, error_pct);
    }
}