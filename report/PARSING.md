
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