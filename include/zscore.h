#pragma once
#include "stats.h"

Dataset* remove_outliers_zscore(Dataset *original, double z_threshold);