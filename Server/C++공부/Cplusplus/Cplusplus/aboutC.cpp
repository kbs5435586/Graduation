#include <iostream>

using namespace std;

int main()
{
	const int num = 10;
	const int* ptr1 = &val1; // 포인터 ptr1을 이용한 값 변경 불가능, 포인터의 주소 변경 가능
	int* const ptr2 = &val2; // 포인터 ptr2를 이용한 값 변경 가능, 포인터의 주소 변경 불가능
	const int* const ptr3 = &val3; // 이렇게 하면 둘다 변경 불가




}