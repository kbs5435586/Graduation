#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main()
{
	// 키보드에서 입력한 문자열을 저장한 후 문자열을 역순으로 출력해봐라

	// char 포인터 사용한 경우
	char* sentence = nullptr;
	cin >> sentence;

	int length = strlen(sentence);

	for (int i = length; i > 0; i--)
	{
		cout << sentence;
	}

	// 스트링 사용한 경우 잘됨
	//void reverse(string * temp);

	//string sentence;
	//getline(cin, sentence);

	//cout << "변경 전 : " << sentence << endl;
	//reverse(&sentence);
	//cout << "변경 후 : " << sentence << endl;

	// char str[] 사용한 경우

	//char str[] = { 0 };
	//int size = sizeof(str);
	//cin >> str;

	//int sizenun = sizeof(str);
	//cout << "변경 전 : " << str << endl;
	//void reverse(char* temp);



	return 0;
}

void reverse(string* temp)
{
	reverse(temp->begin(), temp->end());
}

void reverse(char* temp)
{
	
}
