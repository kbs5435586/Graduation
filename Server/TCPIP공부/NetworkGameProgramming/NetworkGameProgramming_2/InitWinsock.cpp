#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <bitset>

using namespace std;

int main(int argc, char *argv[]) // 실습2-1
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	else
	{
		unsigned short highVer = wsa.wVersion & 255;  // 상위 8비트 주버전 (하위 8비트에 들어감) // 16비트
		unsigned char lowVer = wsa.wVersion >> 8; // 하위 8비트 부버전 (상위 8비트에 들어감) // 

		cout << "주 버전 : " << highVer << " (" << bitset<16>(highVer) << ")" << endl;
		cout << "부 버전 : " << (int)lowVer << " (" << bitset<8>(lowVer) << ")" << endl;

		cout << "wsa.szDescription : " << wsa.szDescription << endl; // 윈속 설명
		cout << "wsa.szSystemStatus : " << wsa.szSystemStatus << endl; // 윈속 상태
	}

	// 윈속 종료
	WSACleanup();
	system("pause");
	return 0;
}