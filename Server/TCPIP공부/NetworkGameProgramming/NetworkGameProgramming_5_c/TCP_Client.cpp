#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
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

int main(int argc, char* argv[])
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);

    int retval;
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    char myFile[256] = {};
    printf("보낼 파일 이름 : ");
    scanf("%s", &myFile);

    FILE* fp = fopen(myFile, "rb");
    int nameSize = strlen(myFile);

    // 파일 이름 데이터 받기(고정 길이)
    retval = send(sock, (char*)&nameSize, sizeof(nameSize), 0);
    if (retval == SOCKET_ERROR)
    {
        err_display("send() name");
        return 0;
    }

    // 파일 이름 데이터 받기(가변 길이)
    retval = send(sock, myFile, nameSize, 0);
    if (retval == SOCKET_ERROR)
    {
        err_display("send() name size");
        return 0;
    }

    printf("[TCP 클라이언트] %d + %d 바이트를 보냈습니다.\n", sizeof(nameSize), retval);

    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp); // 시작부터 현재 위치까지의 바이트수 읽기

    // 파일 데이터 받기(고정 길이)
    retval = send(sock, (char*)&fileSize, sizeof(fileSize), 0);
    if (retval == SOCKET_ERROR)
    {
        err_display("send() data size");
        return 0;
    }

    printf("[TCP 클라이언트] 보낼 파일 크기 : %d 바이트\n", fileSize);

    char buf[BUFSIZE] = {};
    int checkSize = 0;
    int fileSend;
    rewind(fp);

    while (1)
    {
        fileSend = fread(buf, 1, BUFSIZE, fp);
        if (fileSend > 0)
        {
            retval = send(sock, buf, fileSend, 0); // 파일 데이터 받기(가변 길이)
            printf("fileSend size : %d\n", fileSend);
            if (retval == SOCKET_ERROR)
            {
                err_display("send() data");
                break;
            }
            checkSize += fileSend;
        }
        else if (fileSend == 0 && checkSize == fileSize)
        {
            checkSize += fileSend;
            printf("[TCP 클라이언트] 총 %d 바이트를 보냈습니다.\n", checkSize);
            break;
        }
    }
    fclose(fp);
    closesocket(sock);
    WSACleanup();
    return 0;
}