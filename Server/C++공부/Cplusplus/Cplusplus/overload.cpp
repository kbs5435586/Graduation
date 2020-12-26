#include <iostream>

using namespace std;

void swap(int* a, int* b);
void swap(char* a, char* b);
void swap(double* a, double* b);

int simple(int a = 10)
{
	return a + 1;
}

int simple(void)
{
	return 10;
}

int main(void)
{
	int num1 = 20, num2 = 30;
	swap(&num1, &num2);
	cout << num1 << " , " << num2 << endl;

	char ch1 = 'A', ch2 = 'Z';
	swap(&ch1, &ch2);
	cout << ch1 << " , " << ch2 << endl;

	double db11 = 1.111, db12 = 5.555;
	swap(&db11, &db12);
	cout << db11 << " , " << db12 << endl;

	simple();

	return 0;
}


void swap(int* a, int* b)
{
	int temp = *b;
	*b = *a;
	*a = temp;
}

void swap(char* a, char* b)
{
	char temp = *a;
	*b = *a;
	*a = temp;
}

void swap(double* a, double* b)
{
	double temp = *a;
	*b = *a;
	*a = temp;
}