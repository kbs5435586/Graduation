#include "pch.h"

void init_IOCP(SOCKET& sock, HANDLE& iocp)
{
    wcout.imbue(std::locale("korean"));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ::bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)); // �׳� ���ε� ���� C++11�� �ִ� �����Լ��� ����
    listen(sock, SOMAXCONN);

    iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // Ŀ�� ��ü ����, IOCP ��ü ����
}

void init_AcceptEx(SOCKET& sock, HANDLE& iocp,int l_key)
{
    // �񵿱� accept�� �ϷḦ �޾ƾ��� -> iocp�� �޾ƾ��� -> ���� ��Ĺ�� ����������
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), iocp, l_key, 0); // ���� ��Ĺ iocp ��ü�� ���
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OverEx accept_over;
    ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
    accept_over.function = FUNC_ACCEPT;
    accept_over.c_socket = clientSocket;
    // accept���� wsabuf �̿���ϹǷ� �ʱ�ȭ �� �ʿ� ����
    AcceptEx(sock, clientSocket, accept_over.io_buf, NULL,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);
    // clientSocket�� worker_thread�� �����������

    vector<thread> worker_threads;
    for (int i = 0; i < 4; ++i) // ���⿡ �����ھ�� 4 �־��µ� ���� �ھ����ŭ �־ ����
    {
        worker_threads.emplace_back(worker_thread);
    }

    for (auto& t : worker_threads)
    {
        t.join();
    }
}

void close_server(SOCKET& sock)
{
    closesocket(sock);
    WSACleanup();
}