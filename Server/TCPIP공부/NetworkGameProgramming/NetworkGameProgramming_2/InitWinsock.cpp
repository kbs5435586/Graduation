#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <bitset>

using namespace std;

int main(int argc, char *argv[]) // �ǽ�2-1
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	else
	{
		unsigned short highVer = wsa.wVersion & 255;  // ���� 8��Ʈ �ֹ��� (���� 8��Ʈ�� ��) // 16��Ʈ
		unsigned char lowVer = wsa.wVersion >> 8; // ���� 8��Ʈ �ι��� (���� 8��Ʈ�� ��) // 

		cout << "�� ���� : " << highVer << " (" << bitset<16>(highVer) << ")" << endl;
		cout << "�� ���� : " << (int)lowVer << " (" << bitset<8>(lowVer) << ")" << endl;

		cout << "wsa.szDescription : " << wsa.szDescription << endl; // ���� ����
		cout << "wsa.szSystemStatus : " << wsa.szSystemStatus << endl; // ���� ����
	}

	// ���� ����
	WSACleanup();
	system("pause");
	return 0;
}