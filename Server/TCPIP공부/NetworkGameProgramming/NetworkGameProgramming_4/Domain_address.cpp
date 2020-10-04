#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

void err_display(const char* msg, int err_no)
{
	WCHAR* h_message;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&h_message, 0, NULL
	);
	cout << msg;
	wcout << L" 에러 => " << h_message << endl;

	while (true);
	LocalFree(h_message);
}

// 도메인 이름-> IPv4 주소
BOOL GetIPAddr(const char *name)
{
	IN_ADDR addr;
	HOSTENT *ptr = gethostbyname(name);
	if (ptr == NULL)
	{
		err_display("gethostbyname( )", WSAGetLastError());
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET)
		return FALSE;

	cout << "Official Name : " << ptr->h_name << endl;
	for (int i = 0; ptr->h_aliases[i] != NULL; i++)
	{
		cout << "Other Domain Name " << i + 1 << " : " << ptr->h_aliases[i] << endl;
	}

	for (int i = 0; ptr->h_addr_list[i] != NULL; i++)
	{
		memcpy(&addr, ptr->h_addr_list[i], ptr->h_length);
		cout << "Other IP " << i + 1 << " : " << inet_ntoa(addr) << endl;
	}
	cout << endl;
	return TRUE;
}

//IPv4 -> 도메인 이름
BOOL GetDomainName(unsigned long IP, char* name, int namelen)
{
	IN_ADDR addr;
	HOSTENT* ptr = gethostbyaddr((char*)&IP, sizeof(IP), AF_INET);
	if (ptr == NULL) {
		err_display("gethostbyaddr()", WSAGetLastError());
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET)
		return FALSE;

	cout << "Official Name " << ptr->h_name << endl;
	for (int i = 0; ptr->h_aliases[i] != NULL; i++)
	{
		cout << "Other Domain Name " << i + 1 << " : " << ptr->h_aliases[i] << endl;
	}

	for (int i = 0; ptr->h_addr_list[i] != NULL; i++)
	{
		memcpy(&addr, ptr->h_addr_list[i], ptr->h_length);
		cout << "Other IP " << i + 1 << " : " << inet_ntoa(addr) << endl;
	}


	cout << "도메인 이름 (다시 변환 후) = " << ptr->h_name << endl << endl;
	return TRUE;
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	while (true)
	{
		char TESTNAME[30] = { NULL };
		cout << "도메인 이름이나 IP 입력하세요 = ";
		cin >> TESTNAME;

		// 도메인 이름 -> IP 주소
		if (TESTNAME[0] >= 65)
		{
			GetIPAddr(TESTNAME);
		}
		else
		{
			char name[256];
			GetDomainName(inet_addr(TESTNAME), name, sizeof(name));
			// <inet_addr>이 함수가 교수님이 강의자료에서 네트워크 바이트 정렬로 바꿔준다
			// 그래서 이 함수를 이용해서 입력받은 문자열의 IP주소를 네트워크가 읽을 수 있게 빅인디언 방식으로 바꿔주고
			// ip주소랑 포트번호 둘다 빅인디언으로 바꼈다가
			// 다시 읽어올때 <inet_ntoa> 함수를 통해서 다시 컴퓨터가 읽을 수 있게 리틀인디언 방식으로 바꿔준다
		}
	}
	WSACleanup();
	return 0;
}