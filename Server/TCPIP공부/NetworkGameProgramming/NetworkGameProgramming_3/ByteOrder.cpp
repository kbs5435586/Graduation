#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>

using namespace std;

BOOL isLittleEndian(unsigned short check);
BOOL isBigEndian(unsigned short check);

int main(int argc, char* argv[]) // �ǽ� 3-1
{
	unsigned short num = 0x1234; // 2����Ʈ

	isLittleEndian(num);
	isBigEndian(num);

	system("pause");
}

BOOL isLittleEndian(unsigned short check)
{
	if (*((char*)&check) == 0x34) // ���� ����Ʈ�� ���� �о��� (���� ����Ʈ���� ����)
	{
		cout << "It is LittleEndian" << endl;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL isBigEndian(unsigned short check)
{
	if (*((char*)&check) == 0x12) // ���� ����Ʈ�� ���� �о��� (���� ����Ʈ���� ����)
	{
		cout << "It is BigEndian" << endl;
		return TRUE;
	}
	else 
		return FALSE;
}
