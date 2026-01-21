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
    model->bias = 200000.0;
    model->cost = 0.0;
    
    // according to AI we will benefit from initially having random normalized weights rather than zeroed out weights
    init_random();
    for (int i = 0; i < num_features; i++) {
        model->weights[i] = 10000.0 * ((rand() / (double)RAND_MAX) * 2.0 - 1.0);
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

    double *predictions = malloc(n * sizeof(double));
    double *errors = malloc(n * sizeof(double));
    double *abs_errors = malloc(n * sizeof(double));
    double *error_percents = malloc(n * sizeof(double));
    
    if (!predictions || !errors || !abs_errors || !error_percents) {
        printf("Memory allocation failed\n");
        return;
    }

    double y_sum = 0.0;
    double y_min = INFINITY, y_max = -INFINITY;
    
    for (int i = 0; i < n; i++) {
        predictions[i] = predict(x_test->data[i], model);
        errors[i] = predictions[i] - y_test->data[i];
        abs_errors[i] = fabs(errors[i]);
        
        if (y_test->data[i] > 0) {
            error_percents[i] = (abs_errors[i] / y_test->data[i]) * 100.0;
        } else {
            error_percents[i] = 0.0;
        }
        
        y_sum += y_test->data[i];
        if (y_test->data[i] < y_min) y_min = y_test->data[i];
        if (y_test->data[i] > y_max) y_max = y_test->data[i];
    }
    
    double y_mean = y_sum / n;
    
    double mse = 0.0, mae = 0.0, rmse = 0.0;
    double max_abs_error = 0.0, max_error_percent = 0.0;
    double min_abs_error = INFINITY, min_error_percent = INFINITY;
    double error_sum = 0.0, abs_error_sum = 0.0, error_percent_sum = 0.0;
    
    for (int i = 0; i < n; i++) {
        mse += errors[i] * errors[i];
        mae += abs_errors[i];
        error_sum += errors[i];
        abs_error_sum += abs_errors[i];
        error_percent_sum += error_percents[i];
        
        if (abs_errors[i] > max_abs_error) max_abs_error = abs_errors[i];
        if (abs_errors[i] < min_abs_error) min_abs_error = abs_errors[i];
        if (error_percents[i] > max_error_percent) max_error_percent = error_percents[i];
        if (error_percents[i] < min_error_percent) min_error_percent = error_percents[i];
    }
    
    mse /= n;
    mae /= n;
    rmse = sqrt(mse);
    double avg_error = error_sum / n;
    double avg_error_percent = error_percent_sum / n;
    
    double ss_total = 0.0, ss_residual = 0.0;
    for (int i = 0; i < n; i++) {
        ss_total += pow(y_test->data[i] - y_mean, 2);
        ss_residual += pow(predictions[i] - y_test->data[i], 2);
    }
    
    double r2 = (ss_total > 0) ? 1.0 - (ss_residual / ss_total) : 0.0;
    
    int within_10k = 0, within_20k = 0, within_50k = 0;
    int within_10pct = 0, within_20pct = 0, within_30pct = 0;
    
    for (int i = 0; i < n; i++) {
        if (abs_errors[i] <= 10000.0) within_10k++;
        if (abs_errors[i] <= 20000.0) within_20k++;
        if (abs_errors[i] <= 50000.0) within_50k++;
        
        if (error_percents[i] <= 10.0) within_10pct++;
        if (error_percents[i] <= 20.0) within_20pct++;
        if (error_percents[i] <= 30.0) within_30pct++;
    }
    
    double *sorted_abs_errors = malloc(n * sizeof(double));
    double *sorted_error_percents = malloc(n * sizeof(double));
    
    for (int i = 0; i < n; i++) {
        sorted_abs_errors[i] = abs_errors[i];
        sorted_error_percents[i] = error_percents[i];
    }
    
    qsort(sorted_abs_errors, n, sizeof(double), cmp_dbl);
    qsort(sorted_error_percents, n, sizeof(double), cmp_dbl);
    
    double median_error = sorted_abs_errors[n / 2];
    double median_error_percent = sorted_error_percents[n / 2];
    double p75_error = sorted_abs_errors[(int)(n * 0.75)];
    double p90_error = sorted_abs_errors[(int)(n * 0.90)];
    
    printf("\n==============================================\n");
    printf("           MODEL TEST REPORT\n");
    printf("==============================================\n");
    
    printf("\n1. DATASET OVERVIEW:\n");
    printf("   Test samples: %d\n", n);
    printf("   Target range: $%.2f to $%.2f\n", y_min, y_max);
    printf("   Average price: $%.2f\n", y_mean);
    
    printf("\n2. QUALITY:\n");
    printf("   R-squared (R²): %.4f\n", r2);
    
    printf("\n3. ERROR METRICS (in dollars):\n");
    printf("   Mean Absolute Error (MAE):    $%11.2f\n", mae);
    printf("   Root Mean Squared Error (RMSE): $%11.2f\n", rmse);
    printf("   Mean Squared Error (MSE):     $%11.2f\n", mse);
    printf("   Average error (bias):         $%11.2f\n", avg_error);
    printf("   Median absolute error:        $%11.2f\n", median_error);
    
    printf("\n4. ERROR RELATIVE TO AVERAGE PRICE:\n");
    printf("   MAE / Average price:         %9.1f%%\n", (mae / y_mean) * 100);
    printf("   RMSE / Average price:        %9.1f%%\n", (rmse / y_mean) * 100);
    
    printf("\n5. ERROR DISTRIBUTION (percentiles):\n");
    printf("   Min error:                  $%11.2f\n", min_abs_error);
    printf("   25th percentile:            $%11.2f\n", sorted_abs_errors[(int)(n * 0.25)]);
    printf("   Median (50th):              $%11.2f\n", median_error);
    printf("   75th percentile:            $%11.2f\n", p75_error);
    printf("   90th percentile:            $%11.2f\n", p90_error);
    printf("   Max error:                  $%11.2f\n", max_abs_error);
    
    printf("\n6. ACCURACY (dollar amounts):\n");
    printf("   Within $10,000:  %5d/%d (%5.1f%%)\n", within_10k, n, (within_10k * 100.0) / n);
    printf("   Within $20,000:  %5d/%d (%5.1f%%)\n", within_20k, n, (within_20k * 100.0) / n);
    printf("   Within $50,000:  %5d/%d (%5.1f%%)\n", within_50k, n, (within_50k * 100.0) / n);
    
    printf("\n7. PERCENTAGE ERROR ANALYSIS:\n");
    printf("   Average %% error:            %9.1f%%\n", avg_error_percent);
    printf("   Median %% error:             %9.1f%%\n", median_error_percent);
    printf("   Max %% error:                %9.1f%%\n", max_error_percent);
    
    printf("\n8. ACCURACY (percentage):\n");
    printf("   Within 10%% error: %5d/%d (%5.1f%%)\n", within_10pct, n, (within_10pct * 100.0) / n);
    printf("   Within 20%% error: %5d/%d (%5.1f%%)\n", within_20pct, n, (within_20pct * 100.0) / n);
    printf("   Within 30%% error: %5d/%d (%5.1f%%)\n", within_30pct, n, (within_30pct * 100.0) / n);
    
    printf("\n==============================================\n");
    
    free(predictions);
    free(errors);
    free(abs_errors);
    free(error_percents);
    free(sorted_abs_errors);
    free(sorted_error_percents);
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

void show_line_equation(LinearModel *model) {
    printf("\n=== REGRESSION EQUATION ===\n\n");
    printf("Predicted Price = %.2f", model->bias);
    
    for (int i = 0; i < model->num_features; i++) {
        if (model->weights[i] >= 0) {
            printf(" + %.4f * x%d", model->weights[i], i+1);
        } else {
            printf(" - %.4f * x%d", fabs(model->weights[i]), i+1);
        }
    }
    printf("\n\n");
    
    const char *features[] = {
        "scaled_longitude",
        "scaled_latitude",
        "scaled_age",
        "scaled_rooms",
        "scaled_bedrooms",
        "scaled_population",
        "scaled_households",
        "scaled_income"
    };
    
    printf("Where (all features scaled 0-1):\n");
    for (int i = 0; i < model->num_features; i++) {
        printf("  x%d = %s (weight: %+.4f)\n", i+1, features[i], model->weights[i]);
    }
    printf("\n");
}