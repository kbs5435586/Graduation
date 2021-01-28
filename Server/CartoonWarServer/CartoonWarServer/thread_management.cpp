#include "pch.h"

void worker_thread()
{
    while (true)
    {
        DWORD io_byte;
        ULONG_PTR key;
        WSAOVERLAPPED* over;
        GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE); // recv ��� IOCP�� ����
        // io_byte�� 0�� ��� = �� Ŭ�� ������ �����߱� ������ 0����Ʈ�� ���۵ȴ� 
        OverEx* overEx = reinterpret_cast<OverEx*>(over); // �ӽ� Ȯ�� ������ ����ü�� IOCP�� ����� �� ����
        int id = static_cast<int>(key); // �ӽ� ���̵� IOCP Ű��(Ŭ�� id��) ����

        switch (overEx->function) // send, recv, accept ����
        {
        case FUNC_RECV:
        {
            if (0 == io_byte)
                disconnect(id);
            else
            {
                recv_packet_construct(id, io_byte);
                ZeroMemory(&g_clients[id].m_recv_over.over, sizeof(g_clients[id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
                DWORD flags = 0;
                WSARecv(g_clients[id].m_socket, &g_clients[id].m_recv_over.wsabuf, 1, NULL, // ��Ŷ ó���� �ʱ�ȭ ������ �ٽ� recv ȣ��
                    &flags, &g_clients[id].m_recv_over.over, NULL);
            }
        }
        break;

        case FUNC_SEND:
            if (0 == io_byte)
                disconnect(id);

            delete overEx; // Ȯ�� ����ü �ʱ�ȭ�� ���ָ� �ȴ�
            break;

        case FUNC_ACCEPT:
        {
            int user_id = current_User_ID++;

            SOCKET clientSocket = overEx->c_socket;

            CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, user_id, 0); // 
            current_User_ID = current_User_ID % MAX_USER; // 10 �Ѿ� ������ �ɷ����� �뵵
            g_clients[user_id].m_id = user_id; // ���� ���
            g_clients[user_id].m_prev_size = 0; // ���� 0���� �ʱ�ȭ
            g_clients[user_id].m_recv_over.function = FUNC_RECV; // ������ ����ü�� �޴°ɷ� ����
            ZeroMemory(&g_clients[user_id].m_recv_over.over, sizeof(g_clients[user_id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
            g_clients[user_id].m_recv_over.wsabuf.buf = g_clients[user_id].m_recv_over.io_buf; // WSA ���� ��ġ ����
            g_clients[user_id].m_recv_over.wsabuf.len = MAX_BUF_SIZE; // WSA���� ũ�� ����
            g_clients[user_id].m_socket = clientSocket;
            g_clients[user_id].m_x = rand() % WORLD_WIDTH;
            g_clients[user_id].m_x = rand() % WORLD_HEIGHT;

            DWORD flags = 0;
            WSARecv(clientSocket, &g_clients[user_id].m_recv_over.wsabuf, 1, NULL,
                &flags, &g_clients[user_id].m_recv_over.over, NULL); // ������� �ϳ��� Ŭ�� ���� ����̶� recv ȣ���� ������

            clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // ���� ���� Ŭ�� ���� 
            ZeroMemory(&overEx->over, sizeof(overEx->over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
            // accept�� �Ϸ�� ������ �ٽ� accept �޴� �κ��̹Ƿ� overEx �ٽ� ����ص���, �ߺ��ؼ� �Ҹ� �� ����
            AcceptEx(listenSocket, clientSocket, overEx->io_buf, NULL,
                sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &overEx->over);
        }
        break;
        }
    }
}