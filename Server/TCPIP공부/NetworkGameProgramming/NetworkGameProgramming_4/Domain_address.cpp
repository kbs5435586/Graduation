#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>

using namespace std;

#define TESTNAME "www.naver.com" // �ǽ� 3-6

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
	wcout << L" ���� => " << h_message << endl;

	while (true);
	LocalFree(h_message);
}

// ������ �̸�-> IPv4 �ּ�
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

//IPv4 -> ������ �̸�
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

	cout << "������ �̸�(��ȯ ��) = " << TESTNAME << endl;

	// ������ �̸� -> IP �ּ�
	IN_ADDR addr;
	if (GetIPAddr(TESTNAME, &addr))
	{
		// �����̸� ��� ���
		cout << "IP �ּ� (��ȯ ��) = " << inet_ntoa(addr) << endl;

		// IP �ּ� -> ������ �̸�
		char name[256];
		if (GetDomainName(addr, name, sizeof(name)))
		{
			// �����̸� ��� ���
			cout << "������ �̸�(�ٽ� ��ȯ ��)" << name << endl;
		}
	}

	WSACleanup();
	return 0;
}