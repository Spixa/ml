#include "zscore.h"
#include "data.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Dataset* remove_outliers_zscore(Dataset *original, double z_threshold) {
    if (!original || original->count == 0) return NULL;
    
    Dataset *filtered = allocate_dataset();
    
    ColumnStats stats[9];
    calculate_stats(original, stats);
    
    double stds[9];
    for (int col = 0; col < 9; col++) {
        double variance = 0.0;
        int count = 0;
        
        for (int i = 0; i < original->count; i++) {
            double value = get(&original->points[i], col);
            if (!isnan(value)) {
                double diff = value - stats[col].mean;
                variance += diff * diff;
                count++;
            }
        }
        
        if (count > 0) {
            stds[col] = sqrt(variance / count);
        } else {
            stds[col] = 0.0;
        }
    }
    
    printf("\n=== Z-Score Outlier Detection (threshold: %.1fσ) ===\n", z_threshold);
    
    int outliers_per_col[9] = {0};
    
    int removed = 0;
    for (int i = 0; i < original->count; i++) {
        DataPoint *dp = &original->points[i];
        int is_outlier = 0;
        
        // Check key columns with z-score
        int key_columns[] = {3, 5, 7, 8}; // rooms, population, income, value
        
        for (int k = 0; k < 4; k++) {
            int col = key_columns[k];
            double value = get(dp, col);
            
            if (!isnan(value) && stds[col] > 0) {
                double z_score = fabs((value - stats[col].mean) / stds[col]);
                
                if (z_score > z_threshold) {
                    outliers_per_col[col]++;
                    is_outlier = 1;
                }
            }
        }
        
        if (!is_outlier) {
            if (filtered->count >= filtered->cap) {
                expand_dataset(filtered);
            }
            filtered->points[filtered->count] = *dp;
            filtered->count++;
        } else {
            removed++;
        }
    }
    
    printf("\nRemoved %d outliers total\n", removed);
    printf("Kept %d records\n", filtered->count);
    
    return filtered;
}