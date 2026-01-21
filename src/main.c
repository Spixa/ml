#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "model.h"
#include "scaler.h"
#include "stats.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <csv_file> [learning_rate=0.01] [epochs=1000] [train_split=0.8]\n", argv[0]);
        return 1;
    }

    double learning_rate = (argc > 2) ? atof(argv[2]) : 0.01;
    int epochs = (argc > 3) ? atoi(argv[3]) : 1000;
    double train_split = (argc > 4) ? atof(argv[4]) : 0.8;

    train_split = train_split > 1.0 ? 1.0 : train_split;
    
    printf("Loading dataset: %s\n", argv[1]);
    Dataset *dataset = load_dataset(argv[1]);
    if (!dataset) return 1;
    printf("Loaded %d records\n", dataset->count);
    ColumnStats stats[9];
    calculate_stats(dataset, stats);
    impute_missing(dataset, stats);

    Matrix X;
    Vector y;
    prepare_data(dataset, &X, &y);

    MatScaler scaler;
    scale_matrix(&X, &scaler);
    // normalize_matrix(&X, &scaler);
    printf("Features scaled to [0, 1]\n");

    DataSplit split;
    split_data(&X, &y, train_split, &split);
    printf("Data split: %d train, %d test\n", split.train_size, split.test_size);

    LinearModel model;
    init_model(&model, NUM_FEATURES);
    
    printf("\nTraining model (learning_rate=%.4f, epochs=%d)...\n", learning_rate, epochs);
    train(&split.x_train, &split.y_train, &model, learning_rate, epochs);

    test(&split.x_test, &split.y_test, &model);
    show_predictions(&split.x_test, &split.y_test, &model, 15);
    show_line_equation(&model);

    free_dataset(dataset);
    free_matrix(&X);
    free_vector(&y);
    free_split(&split);
    free_model(&model);
    
    return 0;
}