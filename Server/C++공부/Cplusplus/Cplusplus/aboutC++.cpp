#include <iostream>
#include <string>
using namespace std;

struct student {
	char name[10];
	int num;
	int score;
};

//템플릿 함수
template <class T>
T square(T x);

int main()
{
	// const와 포인터 차이

	//const int num = 10;
	//const int* ptr1 = &val1; // 포인터 ptr1을 이용한 값 변경 불가능, 포인터의 주소 변경 가능
	//int* const ptr2 = &val2; // 포인터 ptr2를 이용한 값 변경 가능, 포인터의 주소 변경 불가능
	//const int* const ptr3 = &val3; // 이렇게 하면 둘다 변경 불가

	//====================================================================================================

	// c와 c++ 문자열 입력받는 방법 차이

	//string cpp;
	//char c[256];

	//cin.getline(c, 256);
	//getline(cin, cpp);
	//
	//cout << c << endl;
	//cout << cpp << endl;
	//return 0;

	//====================================================================================================

	// C 스타일 문자열을 string으로 처리하려면
	//string name = "i can do it";
	//char pt[] = "Love";

	////strcpy(name.c_str(), pt); // 이렇게 하면 오류남
	//strcpy(pt, name.c_str());

	//====================================================================================================

	/*
	* 포인터 변수 : 포인터 변수가 기리키는 메모리 주소에 저장된 값
	& 변수 : 변수에 할당된 메모리의 시작 주소

	포인터 사용하려면 필요한 데이터 2가지
	1. 포인터
	2. 포인터가 가리키는 객체
	*/

	//int x = 100;
	//int* pt; // 포인터 변수 pt의 내용은 int형이다

	//pt = &x; // 포인터 변수 pt에 변수 x의 주소 값을 할당하여 x를 가리킨다

	//cout << "x의 메모리 시작 번지 : " << &x << endl;
	//cout << "pt의 메모리 시작 번지 : " << &pt << endl;
	//cout << "pt가 가리키는 메모리 번지에 저장된 데이터 : " << *pt << endl;

	//*1000 = 34; // 불가능
	//*(int*)1000 = 34; // 주소 1000번지가 가리키는 곳의 내용, 이곳에 34를 대입해 저장해라

	//====================================================================================================

	// 자료형을 예측할 수 없는 경우 void* 사용
	//void swap(void* x, void* y);

	//int x(3), y(5);

	//cout << "swap 이전 : " << x << " , " << y << endl;

	//swap(&x, &y);

	//cout << "swap 이후 : " << x << " , " << y << endl;

	// 포인터랑 배열
	//int str[3]; // 이 있을때 str[3] 이랑 *(str+3)이랑 같은 뜻임
	//int* pt;
	//pt = str; // str은 배열명이고 &str[0]이랑 동일함 

	//====================================================================================================

	// 포인터랑 함수
	// 포인터를 사용하면 다른 함수의 변수를 간접으로 접근 가능 (그 변수의 주소값을 가리키니까!)
	// 그래서 배열도 전달 가능, 그리고 처리한 결과값 반환받을때 사용

	//void change(int* p);
	//int num = 1000;
	//
	//change(&num);
	//cout << "변경 후 값: " << num << endl;

	//====================================================================================================

	//구조체의 포인터
	//student Kim = { "Byeon Suk",26,95 };
	//student* pt;
	//pt = &Kim;

	//void changeStruct(student* temp);

	//cout << "변경 전 : " << Kim.name << " , " << Kim.num << " , " << Kim.score << endl;
	//changeStruct(&Kim);
	//cout << "변경 후 : " << Kim.name << " , " << Kim.num << " , " << Kim.score << endl;

	//====================================================================================================

	//템플릿 함수
	int ir = square(8);
	cout << ir;

}

template <class T>
T square(T x)
{
	return x * x;
}

void changeStruct(student* temp)
{
	strcpy_s(temp->name,"Hyeon ho");
	temp->num = 27;
	temp->score = 80;
}

void swap(void* x, void* y)
{
	int temp;
	temp = *(int*)x;
	*(int*)x = *(int*)y;
	*(int*)y = temp;
}

void change(int* p)
{
	cout << "변경 전 값: " << *p << endl;
	*p = 300;
}

