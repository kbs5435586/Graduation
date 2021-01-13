#include "server.h"

void mainServer()
{
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // �׳� ���ε� ���� C++11�� �ִ� �����Լ��� ����
    listen(listenSocket, SOMAXCONN);

    SOCKADDR_IN clientAddr;
    ZeroMemory(&clientAddr, sizeof(clientAddr));
    int c_AddrSize = sizeof(clientAddr);

    while (true) {
        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &c_AddrSize);

        int user_id = current_User_ID++;
        current_User_ID %= MAX_USER; // 10 �Ѿ� ������ �ɷ����� �뵵

        g_clients[user_id].m_id = user_id;
        g_clients[user_id].m_prev_size = 0; // ���� 0���� �ʱ�ȭ
        g_clients[user_id].m_recv_over.op = OP_RECV; // ������ ����ü�� �޴°ɷ� ����
        ZeroMemory(&g_clients[user_id].m_recv_over.over, sizeof(g_clients[user_id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
        g_clients[user_id].m_recv_over.wsabuf.buf = g_clients[user_id].m_recv_over.io_buf; // WSA ���� ��ġ ����
        g_clients[user_id].m_recv_over.wsabuf.len = MAX_BUF_SIZE; // WSA���� ũ�� ����
        g_clients[user_id].m_socket = clientSocket;
        g_clients[user_id].m_x = rand() % WORLD_WIDTH;
        g_clients[user_id].m_x = rand() % WORLD_HEIGHT;

        WSARecv(clientSocket, &g_clients[user_id].m_recv_over.wsabuf, 1, NULL, 
            &flags, g_clients[user_id].m_recv_over.over, NULL);

    }
    closesocket(listenSocket);
    WSACleanup();
}
