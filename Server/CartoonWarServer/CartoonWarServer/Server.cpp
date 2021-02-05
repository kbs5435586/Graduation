#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::error_display(const char* msg, int err_no)
{
    WCHAR* lpMsgBuf; 
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM, 
        NULL, err_no, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    cout << msg;
    wcout << L"���� " << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
}

void Server::recv_packet_construct(int user_id, int io_byte)
{
    int rest_byte = io_byte; // �̸�ŭ ���Ҵ�, �̸�ŭ ó���� ���� ������Ѵ�

    char* p = g_clients[user_id].m_recv_over.io_buf;// ���� �߿��� ��� �κ��� ó���ϰ� �ִ�, ��Ŷ�� ó���Ҽ��� ó���� ��Ŷ ���� �����Ϳ� ������ �ؾ���
    //�츮�� ó���ؾ��� �����Ϳ� ���� ������, ó�� �����ϴ°Ŵϱ� io_buf�� ����ִ� ������ �� �պ��� �����ؾ���
    int packet_size = 0;

    if (0 != g_clients[user_id].m_prev_size) // ������ �޾Ƴ��� �����Ͱ� ������
        packet_size = g_clients[user_id].m_packet_buf[0];

    while (rest_byte > 0) // ó���ؾ��� �����Ͱ� ����������
    {
        if (0 == packet_size) // ���� �츮�� ó���ϴ� ��Ŷ�� ó���϶� -> �����͸� ��Ŷ ������ �� ���� ����Ű�� ����
            packet_size = *p;

        if (packet_size <= rest_byte + g_clients[user_id].m_prev_size) // �������� �����Ŷ� �������� ���ļ� ��Ŷ ������� ũ�ų� ������ ��Ŷ �ϼ�
        {
            memcpy(g_clients[user_id].m_packet_buf + g_clients[user_id].m_prev_size, p, packet_size - g_clients[user_id].m_prev_size); // p�� �ִ°� packet_size��ŭ m_packet_buf�� ����
            
            p += packet_size - g_clients[user_id].m_prev_size;
            rest_byte -= packet_size - g_clients[user_id].m_prev_size;
            packet_size = 0;
            process_packet(user_id, g_clients[user_id].m_packet_buf);
            g_clients[user_id].m_prev_size = 0;
        }
        else // ���ĵ� ��Ŷ ���̺꺸�� �۴�, ��Ŷ �ϼ��� ����Ų��, �������� �����س��� ��������
        {
            memcpy(g_clients[user_id].m_packet_buf + g_clients[user_id].m_prev_size, p, rest_byte); // p�� �ִ°� rest_byte��ŭ m_packet_buf�� ����
            // Ȥ�ö� 2���̻� �޾Ҵµ� ��Ŷ �ϼ� ����Ų ��찡 ���� �� ������ ������ �޾Ƴ��� ũ�� �ں��� �����ؿ��� ����
            g_clients[user_id].m_prev_size += rest_byte;
            rest_byte = 0;
            p += rest_byte; // ó�� �������� �׸�ŭ ������ ��ġ �̵�
        }
    }
}

void Server::process_packet(int user_id, char* buf)
{
	switch (buf[1]) // �츮�� ��Ŷ�� 0�� char size, 1�� char type���� ���������Ƿ�
	{
	case CS_PACKET_LOGIN: // case������ ������ �����Ҷ� �߰�ȣ ����� ������ ������ �ȴ�
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
        enter_game(user_id, packet->name); // ���� �������� ���� ó�� �� �̹� �����ִ� Ŭ�� ���� ����

        // ���� �� �ɶ� socket�̶� connected�� name�� ���ÿ� ��ȣ�������
        // bool connected ���� ������ true�϶� socket�̳� nameó�� �ѹ� ���� ������Ʈ ���� �ʴ�
        // ������ �����ϴٴ� �ǹ�, �׷��� connected�� true �Ǳ� ���� socket, name�� ó���ϴ���
        // true�Ҷ� ���� ���� �ɴ��� �� �� �ϳ��� �ؾ���
	}
	break;
	case CS_PACKET_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
        g_clients[user_id].m_move_time = packet->move_time;
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
	overEx->function = FUNC_SEND; // Ÿ�� ����
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

    for (auto &c : g_clients)
    {
        c.m_cLock.lock();
        if (ST_ACTIVE == c.m_status)
        {
            send_move_packet(c.m_id, user_id); // ����� ��� Ŭ���̾�Ʈ�鿡�� ������ Ŭ���� ��ġ�� ����
        }
        c.m_cLock.unlock();
    }
}

void Server::send_move_packet(int user_id, int mover)
{
    sc_packet_move packet;
    packet.id = mover;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;
    packet.x = g_clients[mover].m_x;
    packet.y = g_clients[mover].m_y; // �̵��� �÷��̾��� ���� ���
    packet.move_time = g_clients[mover].m_move_time;

    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::enter_game(int user_id, char name[])
{
    g_clients[user_id].m_cLock.lock();

    strcpy_s(g_clients[user_id].m_name, name);
    g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // �������� NULL �־��ִ� ó��
    send_login_ok_packet(user_id); // ���� ������ �÷��̾� �ʱ�ȭ ���� ������

    for (int i = 0; i < MAX_USER; i++) 
    {
        if (user_id == i)
            continue;
        g_clients[i].m_cLock.lock();
        if (ST_ACTIVE == g_clients[i].m_status) // �̹� ���� ���� Ŭ������׸�
        {
            if (user_id != i) // �� �ڽ����� send_enter_packet ���� �ʿ䰡 ����, ���� ���Դٴ°� �ٸ� Ŭ�� �˸��� ��Ŷ��
            {
                send_enter_packet(user_id, i); // ���� ������ Ŭ�󿡰� �̹� �������� Ŭ�� �������� ���� 
                send_enter_packet(i, user_id); // �̹� ������ �÷��̾�鿡�� ���� ������ Ŭ������ ����
            }
        }
        g_clients[i].m_cLock.unlock();
    }
    g_clients[user_id].m_cLock.unlock();
    // true == g_clients[i].m_isConnected �ϹǷ� ������ ����� unlock �ؾ��ϴµ� �ȿ� ������ �ʹ� ŭ
    g_clients[user_id].m_status = ST_ACTIVE; // �ٸ� Ŭ������� ���� ���� ������ �������� ST_ACTIVE�� �ٲ��ֱ�
}

void Server::initalize_clients()
{
    for (int i = 0; i < MAX_USER; ++i)
    {
        g_clients[i].m_id = i; // ���� ���
        g_clients[i].m_status = ST_FREE; // ����� ��Ƽ������ �ϱ����� �̱۽������϶� ����ϴ� �Լ�, �� ���ʿ�
    }
}

void Server::send_enter_packet(int user_id, int other_id)
{
    sc_packet_enter packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ENTER;
    packet.x = g_clients[other_id].m_x;
    packet.y = g_clients[other_id].m_y;
    strcpy_s(packet.name, g_clients[other_id].m_name);
    packet.o_type = O_PLAYER; // �ٸ� �÷��̾���� ���� ����

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::disconnect(int user_id)
{
    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_status = ST_ALLOC; // ���⼭ free �ع����� �Ʒ����� �����߿� �ٸ� Ŭ�� �Ҵ�ɼ��� ����
    send_leave_packet(user_id, user_id); // �� �ڽ�
    closesocket(g_clients[user_id].m_socket);

    for (auto& c : g_clients) // ����Ǿ��ִ� Ŭ���̾�Ʈ�鿡�� ���� Ŭ�� �����ٰ� �˸�
    {
        if (user_id == c.m_id)
            continue;

        c.m_cLock.lock();
        if (ST_ACTIVE == c.m_status)
        {
            send_leave_packet(c.m_id, user_id);
        }
        c.m_cLock.unlock();
    }
    g_clients[user_id].m_status = ST_FREE; // ��� ó���� ������ free�ؾ���
    g_clients[user_id].m_cLock.unlock();
}

void Server::send_leave_packet(int user_id, int other_id)
{
    sc_packet_leave packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_LEAVE;

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::worker_thread()
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
            // ����� �� �����尡 accept ������ ���� AcceptEx�� ȣ���ϰ� �س��� ���� �����尡 ���� ���� ����
            int user_id = -1;
            for (int i = 0; i < MAX_USER; ++i)
            {
                lock_guard <mutex> guardLock{ g_clients[i].m_cLock }; // �Լ����� lock �Ҷ� ����
                // �� cLock�� ���� �ɰ� �����尡 ���� ��Ͽ��� ���������� unlock���ְ� ���� �������� unlock-lock ����
                if (ST_FREE == g_clients[i].m_status) // ���� ��ü ���鼭 ���� �����Ѿ� id �ο�
                {
                    g_clients[i].m_status = ST_ALLOC;
                    user_id = i;
                    break; // �Ҵ� ������ ��ü ������ break
                }
            }

            SOCKET clientSocket = overEx->c_socket;

            if (-1 == user_id) // ���� ��� ��ü ���Ҵµ� �Ҵ� ������ ��ü�� ������
                closesocket(clientSocket); //send_login_fail_packet();
            else
            {
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, user_id, 0); // IOCP ��ü�� �������� ��� �� ��ü �ʱ�ȭ

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
            }
            // ���⼭���� ���ο� Ŭ�� ���� accept
            clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // ���� ���� Ŭ�� ���� 
            overEx->c_socket = clientSocket; // ���� ���� Ŭ��ϱ� �� ���� ������ Ȯ�� �������� �־������
            ZeroMemory(&overEx->over, sizeof(overEx->over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
            // accept�� �Ϸ�� ������ �ٽ� accept �޴� �κ��̹Ƿ� overEx �ٽ� ����ص���, �ߺ��ؼ� �Ҹ� �� ����
            AcceptEx(listenSocket, clientSocket, overEx->io_buf, NULL,
                sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &overEx->over);
        }
        break;
        }
    }
}

void Server::mainServer()
{
    wcout.imbue(std::locale("korean"));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // �׳� ���ε� ���� C++11�� �ִ� �����Լ��� ����
    listen(listenSocket, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // Ŀ�� ��ü ����, IOCP ��ü ����
    initalize_clients(); // Ŭ���̾�Ʈ ������ �ʱ�ȭ

     // �񵿱� accept�� �ϷḦ �޾ƾ��� -> iocp�� �޾ƾ��� -> ���� ��Ĺ�� ����������
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), g_iocp, LISTEN_KEY, 0); // ���� ��Ĺ iocp ��ü�� ���
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OverEx accept_over;
    ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
    accept_over.function = FUNC_ACCEPT;
    accept_over.c_socket = clientSocket; // clientSocket�� worker_thread�� �����������
    // accept���� wsabuf �̿���ϹǷ� �ʱ�ȭ �� �ʿ� ����
    AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);

    vector<thread> worker_threads;
    for (int i = 0; i < 4; ++i) // ���⿡ �����ھ�� 4 �־��µ� ���� �ھ����ŭ �־ ����
    {
        worker_threads.emplace_back([this]() {this->worker_thread(); });
    }

    for (auto& t : worker_threads)
    {
        t.join();
    }
    closesocket(listenSocket);
    WSACleanup();
}