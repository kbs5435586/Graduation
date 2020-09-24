#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>

using namespace std;

#define TESTNAME "www.naver.com" // 실습 3-6

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
BOOL GetIPAddr(const char *name, IN_ADDR *addr)
{
	HOSTENT *ptr = gethostbyname(name);
	if (ptr == NULL)
	{
		err_display("gethostbyname( )", WSAGetLastError());
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET)
		return FALSE;
	memcpy(addr, ptr->h_addr_list, ptr->h_length);
	return TRUE;
}

//IPv4 -> 도메인 이름
BOOL GetDomainName(IN_ADDR addr, char* name, int namelen)
{
	HOSTENT* ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display("gethostbyaddr()", WSAGetLastError());
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET)
		return FALSE;
	strncpy(name, ptr->h_name, namelen);
	return TRUE;
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	cout << "도메인 이름(변환 전) = " << TESTNAME << endl;

	// 도메인 이름 -> IP 주소
	IN_ADDR addr;
	if (GetIPAddr(TESTNAME, &addr))
	{
		// 성공이면 결과 출력
		cout << "IP 주소 (변환 후) = " << inet_ntoa(addr) << endl;

		// IP 주소 -> 도메인 이름
		char name[256];
		if (GetDomainName(addr, name, sizeof(name)))
		{
			// 성공이면 결과 출력
			cout << "도메인 이름(다시 변환 후)" << name << endl;
		}
	}

	WSACleanup();
	return 0;
}