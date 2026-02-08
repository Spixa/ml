### خلاصه  Testing
از چند بخش کلیدی تشکیل شده
1. محاسبه آماره های کلیدی: صدک های خطا `MSE MAPE MAE RMSE‍` $R^2$
2. انتخاب ۱۵ داده از آرایه test و نشان دادن آنها به عنوان Demo
3. نشان دادن معادله خط 
4. صدا زدن ‍`gnuplot` برای رسم نمودار


### بررسی یک گزارش خروجی مدل خطی
```
==============================================
           Model test report
==============================================
Test samples: 3546
Target range: $14999.00 to $420200.00
Average price: $180168.08
R-squared (R²): 0.6146

Error metrics (in dollars):
   Mean Absolute Error (MAE):    $   41528.73
   Root Mean Squared Error (RMSE): $   54499.01
   Mean Squared Error (MSE):     $2970142483.91
   Average error (bias):         $     390.69
   Median absolute error:        $   33770.45

Error distribution (percentiles):
   Min error:                  $       7.63
   25th percentile:            $   15863.36
   Median (50th):              $   33770.45
   75th percentile:            $   57141.93
   90th percentile:            $   84667.27
   Max error:                  $  298857.81

Accuracy:
   Within $10,000:    559/3546 ( 15.8%)
   Within $20,000:   1110/3546 ( 31.3%)
   Within $50,000:   2442/3546 ( 68.9%)

Error percentages:
   Average % error:                 27.9%
   Median % error:                  20.5%

MAPE Accuracy:
   100 - MAPE: 72.12%

==============================================

=== Sample Predictions ===
Index  Actual       Predicted    Error        Error %   
------ ------------ ------------ ------------ ----------
0      $252000      $224258      $-27742      11.0     %
1      $98800       $101898      $+3098       3.1      %
2      $336000      $252616      $-83384      24.8     %
3      $189600      $228063      $+38463      20.3     %
4      $54800       $79033       $+24233      44.2     %
5      $181300      $226696      $+45396      25.0     %
6      $161200      $201889      $+40689      25.2     %
7      $207000      $206253      $-747        0.4      %
8      $231400      $258953      $+27553      11.9     %
9      $161300      $227258      $+65958      40.9     %
10     $330500      $310087      $-20413      6.2      %
11     $258000      $283872      $+25872      10.0     %
12     $211900      $242226      $+30326      14.3     %
13     $231000      $215790      $-15210      6.6      %
14     $162500      $189355      $+26855      16.5     %

=== REGRESSION EQUATION ===

Predicted Price = 183820.00 - 119848.7004 * x1 - 119513.3536 * x2 + 16796.4182 * x3 - 25100.8529 * x4 + 35620.9640 * x5 - 39720.4823 * x6 + 32612.7710 * x7 + 76892.0366 * x8 - 3352.1726 * x9

Where (all features robust scaled):
  x1 = scaled longitude (weight: -119848.7004)
  x2 = scaled latitude (weight: -119513.3536)
  x3 = scaled age (weight: +16796.4182)
  x4 = scaled rooms (weight: -25100.8529)
  x5 = scaled bedrooms (weight: +35620.9640)
  x6 = scaled population (weight: -39720.4823)
  x7 = scaled households (weight: +32612.7710)
  x8 = scaled income (weight: +76892.0366)
  x9 = scaled ocean proximity (weight: -3352.1726)

Calling gnuplot to graph actual vs predicted...
```
![[_plot.png]]

