#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>

using namespace std;

BOOL isLittleEndian(unsigned short check);
BOOL isBigEndian(unsigned short check);

int main(int argc, char* argv[]) // 실습 3-1
{
	unsigned short num = 0x1234; // 2바이트

	isLittleEndian(num);
	isBigEndian(num);

	system("pause");
}

BOOL isLittleEndian(unsigned short check)
{
	if (*((char*)&check) == 0x34) // 하위 바이트를 먼저 읽었다 (하위 바이트부터 저장)
	{
		cout << "It is LittleEndian" << endl;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL isBigEndian(unsigned short check)
{
	if (*((char*)&check) == 0x12) // 상위 바이트를 먼저 읽었다 (상위 바이트부터 저장)
	{
		cout << "It is BigEndian" << endl;
		return TRUE;
	}
	else 
		return FALSE;
}
