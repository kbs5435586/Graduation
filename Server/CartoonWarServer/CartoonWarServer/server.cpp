#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::mainServer()
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
        {
            process_packet(id, overEx->io_buf); // ��Ŷ ó�� ��ƾ
            ZeroMemory(&g_clients[id].m_recv_over.over, sizeof(g_clients[id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
            DWORD flags = 0;
            WSARecv(g_clients[id].m_socket, &g_clients[id].m_recv_over.wsabuf, 1, NULL, // ��Ŷ ó���� �ʱ�ȭ ������ �ٽ� recv ȣ��
                &flags, &g_clients[id].m_recv_over.over, NULL);
        }
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
            ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
            AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL,
                sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);
        }
        break;
        }
    }
    closesocket(listenSocket);
    WSACleanup();
}

void Server::process_packet(int user_id, char* buf)
{
	switch (buf[1]) // �츮�� ��Ŷ�� 0�� char size, 1�� char type���� ���������Ƿ�
	{
	case CS_PACKET_LOGIN: // case������ ������ �����Ҷ� �߰�ȣ ����� ������ ������ �ȴ�
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
		strcpy_s(g_clients[user_id].m_name, packet->name);
		g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // Ȥ�� �𸣴ϱ� �����?

		send_login_ok_packet(user_id);
	}
	break;
	case CS_PACKET_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		do_move(user_id, packet->direction);
	}
	break;
	default:
		cout << "Unknown Packet Type Error\n";
		DebugBreak();
		exit(-1);
	}
}

void Server::send_login_ok_packet(int user_id)
{
	sc_packet_login_ok packet;
	packet.exp = 0;
	packet.hp = 0;
	packet.id = user_id;
	packet.level = 0;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.x = g_clients[user_id].m_x;
	packet.y = g_clients[user_id].m_y;

	send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::send_packet(int user_id, void* packet)
{
	char* buf = reinterpret_cast<char*>(packet);

	// m_recv_over�� recv ���� ������ ����ü�̹Ƿ� ���� �ȵȴ�
	// �׳� OverEx overex�� ������ ����ü �����ؼ� ���°͵� �ȵȴ�, ���� ������ �Լ� ������ ������ ��������� -> �Ҵ�޾ƾ���
	OverEx* overEx = new OverEx; // Ȯ�� ������ �Ҵ�
	overEx->op = OP_SEND; // Ÿ�� ����
	ZeroMemory(&overEx->over, sizeof(overEx->over)); // ������ ����ü �ʱ�ȭ
	memcpy(overEx->io_buf, buf, buf[0]); // IOCP���ۿ� ��Ŷ ������ ��Ŷ ũ�⸸ŭ ����
	overEx->wsabuf.buf = overEx->io_buf; // WSA���ۿ� IOCP���� ����
	overEx->wsabuf.len = buf[0]; // ���� ������ ����

	WSASend(g_clients[user_id].m_socket, &overEx->wsabuf, 1, NULL, 0, &overEx->over, NULL);
}

void Server::do_move(int user_id, char direction)
{
    int x = g_clients[user_id].m_x;
    int y = g_clients[user_id].m_y;

    switch (direction)
    {
    case D_UP:
        if (y > 0)
            y--;
        break;

    case D_DOWN:
        if (y < WORLD_HEIGHT-1)
            y++;
        break;

    case D_LEFT:
        if (x > 0)
            x--;
        break;

    case D_RIGHT:
        if (x < WORLD_WIDTH - 1)
            x++;
        break;
    default:
        cout << "Unknown Direction From cs_move_packet !\n";
        DebugBreak();
        exit(-1);
    }
    g_clients[user_id].m_x = x;
    g_clients[user_id].m_y = y;
    send_move_packet(user_id);
}

void Server::send_move_packet(int user_id)
{
    sc_packet_move packet;
    packet.id = user_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;
    packet.x = g_clients[user_id].m_x;
    packet.y = g_clients[user_id].m_y;

    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}