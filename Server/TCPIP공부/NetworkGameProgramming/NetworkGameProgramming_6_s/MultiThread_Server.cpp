#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    512

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
int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;

        left -= received;
        ptr += received;
    }

    return (len - left);
}

HANDLE hReadEvent;

DWORD WINAPI Client_Thread(LPVOID arg)
{
    DWORD event_retval;
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE] = {};

    addrlen = sizeof(clientaddr);
    //getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

    while (1)
    {
        event_retval = WaitForSingleObject(hReadEvent, INFINITE);
        if (event_retval != WAIT_OBJECT_0) break;

        char fileName[256] = {};
        int nameSize;
        int fileSize;

        retval = recvn(client_sock, (char*)&nameSize, sizeof(nameSize), 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv() name");
            break;
        }
        else if (retval == 0)
            break;

        retval = recvn(client_sock, fileName, nameSize, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv() name size");
            break;
        }
        else if (retval == 0)
            break;

        retval = recvn(client_sock, (char*)&fileSize, sizeof(fileSize), 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv() data size");
            break;
        }
        else if (retval == 0)
            break;

        FILE* fp = fopen(fileName, "wb");
        int checkSize = 0;

        while (1)
        {
            retval = recvn(client_sock, buf, BUFSIZE, 0);
            if (retval == SOCKET_ERROR) 
            {
                err_display("recv() data");
                break;
            }
            else if (retval == 0)
                break;
            else
            {
                fwrite(buf, 1, retval, fp);
                checkSize += retval;
                printf("%s 송신률 : %f%%\n", fileName, ((float)checkSize / (float)fileSize) * 100.f);
            }

            if (checkSize == fileSize)
            {
                printf("파일 제대로 전송 완료!\n");
                printf("받은 파일 크기 : %d\n\n", checkSize);
                break;
            }
        }
        fclose(fp);
    }
    closesocket(client_sock);
    printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
        inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    SetEvent(hReadEvent); // 비신호(초록불) -> 신호 (빨간불!)

    return 0;
}

int main(int argc, char* argv[])
{
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    
    HANDLE hThread;
    hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hReadEvent == NULL) return 1;

    while (1) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        hThread = CreateThread(NULL, 0, Client_Thread, (LPVOID)client_sock, 0, NULL);
        WaitForSingleObject(hReadEvent, INFINITE);
        if (hThread == NULL)
            closesocket(client_sock);
        else
            CloseHandle(hThread);
    }

    closesocket(listen_sock);

    WSACleanup();
    return 0;
}
