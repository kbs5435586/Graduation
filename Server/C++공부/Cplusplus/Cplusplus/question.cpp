#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main()
{
	// Ű���忡�� �Է��� ���ڿ��� ������ �� ���ڿ��� �������� ����غ���

	// char ������ ����� ���
	char* sentence = nullptr;
	cin >> sentence;

	int length = strlen(sentence);

	for (int i = length; i > 0; i--)
	{
		cout << sentence;
	}

	// ��Ʈ�� ����� ��� �ߵ�
	//void reverse(string * temp);

	//string sentence;
	//getline(cin, sentence);

	//cout << "���� �� : " << sentence << endl;
	//reverse(&sentence);
	//cout << "���� �� : " << sentence << endl;

	// char str[] ����� ���

	//char str[] = { 0 };
	//int size = sizeof(str);
	//cin >> str;

	//int sizenun = sizeof(str);
	//cout << "���� �� : " << str << endl;
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
