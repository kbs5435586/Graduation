#include "pch.h"

void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

DWORD WINAPI RecvThread(LPVOID arg)
{

}

DWORD WINAPI UpdateThread(LPVOID arg)
{

}

int main(int argc, char* argv[])
{
    int retval;
    int checkClient = 0;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket() error");

    SOCKADDR_IN server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9000);

    retval = bind(listen_sock, (SOCKADDR*)&server_addr, sizeof(server_addr));
    if (retval == SOCKET_ERROR) err_display("bind( ) error");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_display("listen() error");

    SOCKET client_sock;
    SOCKADDR_IN client_addr;
    ZeroMemory(&client_addr, sizeof(client_addr));
    int addrsize = sizeof(client_addr);

    HANDLE recv_thread;
    HANDLE update_thread;

    while (1)
    {
        if (checkClient == 3)
            cout << "Full Client\n";
        else
        {
            client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &addrsize);
            if (client_sock == SOCKET_ERROR)
            {
                err_display("accept() error");
                break;
            }
            printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }

        recv_thread = CreateThread(NULL, 0, RecvThread, (LPVOID)client_sock, 0, NULL);
        update_thread = CreateThread(NULL, 0, UpdateThread, (LPVOID)client_sock, 0, NULL);

    }
}