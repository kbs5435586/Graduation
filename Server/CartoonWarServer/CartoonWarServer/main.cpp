#include "pch.h"

ClientInfo g_clients[MAX_USER];
int current_User_ID = 0;
HANDLE g_iocp;
SOCKET listenSocket; // �������� �ѹ� �������� �ȹٲ�� �� (�����ͷ��̽�X, �����ص� ����)

int LISTEN_KEY = 999;

void error_display(const char* msg, int err_no)
{
    WCHAR* lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    cout << msg;
    wcout << L"���� " << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
}

int main()
{
    initalize_clients(); // Ŭ���̾�Ʈ ������ �ʱ�ȭ

  
	return 0;
}