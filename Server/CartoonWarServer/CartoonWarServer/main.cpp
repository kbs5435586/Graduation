#include "server.h"

int main()
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

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // Ŀ�� ��ü ����, IOCP ��ü ����

     // �񵿱� accept�� �ϷḦ �޾ƾ��� -> iocp�� �޾ƾ��� -> ���� ��Ĺ�� ����������
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), g_iocp, LISTEN_KEY, 0); // ���� ��Ĺ iocp ��ü�� ���
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OverEx accept_over;
    ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
    accept_over.op = OP_ACCEPT;
    // accept���� wsabuf �̿���ϹǷ� �ʱ�ȭ �� �ʿ� ����
    AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL, 
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over); 
   
    while (true) 
    {
        DWORD io_byte;
        ULONG_PTR key;
        WSAOVERLAPPED* over;
        GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE); // recv ��� IOCP�� ����

        OverEx* overEx = reinterpret_cast<OverEx*>(over); // �ӽ� Ȯ�� ������ ����ü�� IOCP�� ����� �� ����
        int id = static_cast<int>(key); // �ӽ� ���̵� IOCP Ű��(Ŭ�� id��) ����
        switch (overEx->op) // send, recv, accept ����
        {
        case OP_RECV:
            process_packet(id, overEx->io_buf); // ��Ŷ ó�� ��ƾ
            ZeroMemory(&g_clients[id].m_recv_over.over, sizeof(g_clients[id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
            WSARecv(g_clients[id].m_socket, &g_clients[id].m_recv_over.wsabuf, 1, NULL, // ��Ŷ ó���� �ʱ�ȭ ������ �ٽ� recv ȣ��
                &flags, &g_clients[id].m_recv_over.over, NULL);
            break;

        case OP_SEND:
            delete overEx; // Ȯ�� ����ü �ʱ�ȭ�� ���ָ� �ȴ�
            break;

        case OP_ACCEPT:
        {
            int user_id = current_User_ID++;
            current_User_ID %= MAX_USER; // 10 �Ѿ� ������ �ɷ����� �뵵

            CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, user_id, 0); // 

            g_clients[user_id].m_id = user_id; // ���� ���
            g_clients[user_id].m_prev_size = 0; // ���� 0���� �ʱ�ȭ
            g_clients[user_id].m_recv_over.op = OP_RECV; // ������ ����ü�� �޴°ɷ� ����
            ZeroMemory(&g_clients[user_id].m_recv_over.over, sizeof(g_clients[user_id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
            g_clients[user_id].m_recv_over.wsabuf.buf = g_clients[user_id].m_recv_over.io_buf; // WSA ���� ��ġ ����
            g_clients[user_id].m_recv_over.wsabuf.len = MAX_BUF_SIZE; // WSA���� ũ�� ����
            g_clients[user_id].m_socket = clientSocket;
            g_clients[user_id].m_x = rand() % WORLD_WIDTH;
            g_clients[user_id].m_x = rand() % WORLD_HEIGHT;

            DWORD flags = 0;
            WSARecv(clientSocket, &g_clients[user_id].m_recv_over.wsabuf, 1, NULL,
                &flags, &g_clients[user_id].m_recv_over.over, NULL); // ������� �ϳ��� Ŭ�� ���� ����̶� recv ȣ���� ������

            SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // ���� ���� Ŭ�� ���� 
            ZeroMemory(&accept_over.over,sizeof(accept_over.over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
            AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL,
                sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);
        }
            break;
        }
    }
    closesocket(listenSocket);
    WSACleanup();
	return 0;
}