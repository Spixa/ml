#include "plot.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

static void write_data_to_file(const char *filename, double *x, double *y, int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%f %f\n", x[i], y[i]);
    }
    fclose(fp);
}

void plot_actual_vs_predicted(Vector *actual, Vector *predicted, const char *title) {
    if (actual->size == 0 || predicted->size == 0) {
        printf("No data to plot\n");
        return;
    }
    
    int n = actual->size;
    
    double *actual_data = actual->data;
    double *predicted_data = predicted->data;

    double min_val = INFINITY, max_val = -INFINITY;
    for (int i = 0; i < n; i++) {
        if (actual_data[i] < min_val) min_val = actual_data[i];
        if (actual_data[i] > max_val) max_val = actual_data[i];
        if (predicted_data[i] < min_val) min_val = predicted_data[i];
        if (predicted_data[i] > max_val) max_val = predicted_data[i];
    }
    
    double margin = (max_val - min_val) * 0.05;
    min_val -= margin;
    max_val += margin;
    
    char data_file[] = "/tmp/plot_data_XXXXXX";
    mkstemp(data_file);
    write_data_to_file(data_file, actual_data, predicted_data, n);
    
    FILE *gp = popen("gnuplot -persist", "w");
    if (!gp) {
        printf("Warning: Could not open gnuplot. Make sure gnuplot is installed.\n");
        remove(data_file);
        return;
    }
    
    fprintf(gp, "set terminal qt size 1200,720 enhanced font 'Helvetica,12'\n");
    fprintf(gp, "set title '%s'\n", title ? title : "Actual vs Predicted");
    fprintf(gp, "set xlabel 'Actual Price ($)'\n");
    fprintf(gp, "set ylabel 'Predicted Price ($)'\n");
    fprintf(gp, "set xrange [%f:%f]\n", min_val, max_val);
    fprintf(gp, "set yrange [%f:%f]\n", min_val, max_val);
    fprintf(gp, "set grid\n");
    fprintf(gp, "set size square\n");
    fprintf(gp, "set key top left\n");    
    fprintf(gp, "plot '%s' with points pointtype 7 pointsize 0.5 lc rgb '#0066CC' title 'Data points', \\\n", data_file);
    fprintf(gp, "     x with lines lw 2 lc rgb '#FF3333' title 'Perfect prediction (y=x)'\n");
    
    pclose(gp);
    remove(data_file);
}