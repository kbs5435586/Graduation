#include "pch.h"

ClientInfo g_clients[MAX_USER];
int current_User_ID = 0;
HANDLE g_iocp;
SOCKET listenSocket; // 서버에서 한번 정해지면 안바뀌는 값 (데이터레이스X, 전역해도 무방)

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
    wcout << L"에러 " << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
}

int main()
{
    initalize_clients(); // 클라이언트 정보들 초기화

  
	return 0;
}