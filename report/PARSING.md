
## data.c 
هدف اصلی این فایل خواندن خط به خط یک فایل .csv هستش و تبدیل کل این فایل به یک ساختاری به نام `Dataset‍‍` که به شکل زیر است:
```c
typedef struct {
	DataPoint *points;
	int count;
	int cap;
} Dataset;
```

که هر `DataPoint` به این شکل تعریف شده:
```c
typedef struct {
	double longitude;
	double latitude;
	double median_house_age;
	double total_rooms;
	double total_bedrooms;
	double population;
	double households;
	double median_income;
	double median_house_value;
	double ocean_proximity; // encoded with labels
} DataPoint;
```

یعنی در نهایت ما به یک آرایه ای از این نقاط می رسیم که کار با آنها بسیار ساده تر است

## انکود کردن داده کیفی `ocean_proximity`
از روش لیبل انکودینگ استفاده می کنیم: بعد از کشف رابطه صعودی قیمت از هوش مصنوعی بین 5 حالت مختلف ما از 0.2 تا 1.0 آنها را مقدار دهی می کنیم با این Mapping:

```
"<1H OCEAN" -> 0.2
"INLAND" -> 0.4
"NEAR OCEAN" -> 0.6
"NEAR BAY -> "0.8"
"ISLAND" = "1.0"
```

روش بهتر استفاده از `One Hot Encoding` هستش که از طریق 3 ocean proximity dummy  قابل انجام بود ولی ما ازش استفاده نکردیم. بیشتر بخاطر این که ما از این روش نا امن برای به دست آوردن ستون داخل هر سطر استفاده می کنیم:
```c
double get(const DataPoint *dp, int col) {
	return *((double*)dp + col);
}
```
که از ویژگی pad شده بودن ساختار ها در C استفاده می کنه و برای این روش انکودینگ یکم گیج کننده می شد