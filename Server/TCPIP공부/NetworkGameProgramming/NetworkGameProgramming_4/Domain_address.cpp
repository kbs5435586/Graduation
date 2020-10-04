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
	wcout << L" ���� => " << h_message << endl;

	while (true);
	LocalFree(h_message);
}

// ������ �̸�-> IPv4 �ּ�
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

//IPv4 -> ������ �̸�
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


	cout << "������ �̸� (�ٽ� ��ȯ ��) = " << ptr->h_name << endl << endl;
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
		cout << "������ �̸��̳� IP �Է��ϼ��� = ";
		cin >> TESTNAME;

		// ������ �̸� -> IP �ּ�
		if (TESTNAME[0] >= 65)
		{
			GetIPAddr(TESTNAME);
		}
		else
		{
			char name[256];
			GetDomainName(inet_addr(TESTNAME), name, sizeof(name));
			// <inet_addr>�� �Լ��� �������� �����ڷῡ�� ��Ʈ��ũ ����Ʈ ���ķ� �ٲ��ش�
			// �׷��� �� �Լ��� �̿��ؼ� �Է¹��� ���ڿ��� IP�ּҸ� ��Ʈ��ũ�� ���� �� �ְ� ���ε�� ������� �ٲ��ְ�
			// ip�ּҶ� ��Ʈ��ȣ �Ѵ� ���ε������ �ٲ��ٰ�
			// �ٽ� �о�ö� <inet_ntoa> �Լ��� ���ؼ� �ٽ� ��ǻ�Ͱ� ���� �� �ְ� ��Ʋ�ε�� ������� �ٲ��ش�
		}
	}
	WSACleanup();
	return 0;
}