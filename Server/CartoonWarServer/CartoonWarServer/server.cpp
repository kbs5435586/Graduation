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

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // 그냥 바인드 쓰면 C++11에 있는 내장함수가 써짐
    listen(listenSocket, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // 커널 객체 생성, IOCP 객체 선언

     // 비동기 accept의 완료를 받아야함 -> iocp로 받아야함 -> 리슨 소캣을 등록해줘야함
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), g_iocp, LISTEN_KEY, 0); // 리슨 소캣 iocp 객체에 등록
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OverEx accept_over;
    ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept용 확장 오버랩 구조체 초기화
    accept_over.op = OP_ACCEPT;
    // accept에선 wsabuf 이용안하므로 초기화 할 필요 없음
    AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);

    while (true)
    {
        DWORD io_byte;
        ULONG_PTR key;
        WSAOVERLAPPED* over;
        GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE); // recv 결과 IOCP에 저장

        OverEx* overEx = reinterpret_cast<OverEx*>(over); // 임시 확장 오버랩 구조체에 IOCP에 저장된 값 대입
        int id = static_cast<int>(key); // 임시 아이디에 IOCP 키값(클라 id값) 대입
        switch (overEx->op) // send, recv, accept 결정
        {
        case OP_RECV:
        {
            process_packet(id, overEx->io_buf); // 패킷 처리 루틴
            ZeroMemory(&g_clients[id].m_recv_over.over, sizeof(g_clients[id].m_recv_over.over)); // 오버랩 구조체 초기화
            DWORD flags = 0;
            WSARecv(g_clients[id].m_socket, &g_clients[id].m_recv_over.wsabuf, 1, NULL, // 패킷 처리랑 초기화 끝나면 다시 recv 호출
                &flags, &g_clients[id].m_recv_over.over, NULL);
        }
        break;

        case OP_SEND:
            delete overEx; // 확장 구조체 초기화만 해주면 된다
            break;

        case OP_ACCEPT:
        {
            int user_id = current_User_ID++;
            current_User_ID %= MAX_USER; // 10 넘어 갔을때 걸러내는 용도

            CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, user_id, 0); // 

            g_clients[user_id].m_id = user_id; // 유저 등록
            g_clients[user_id].m_prev_size = 0; // 버퍼 0으로 초기화
            g_clients[user_id].m_recv_over.op = OP_RECV; // 오버랩 구조체에 받는걸로 설정
            ZeroMemory(&g_clients[user_id].m_recv_over.over, sizeof(g_clients[user_id].m_recv_over.over)); // 오버랩 구조체 초기화
            g_clients[user_id].m_recv_over.wsabuf.buf = g_clients[user_id].m_recv_over.io_buf; // WSA 버퍼 위치 설정
            g_clients[user_id].m_recv_over.wsabuf.len = MAX_BUF_SIZE; // WSA버퍼 크기 설정
            g_clients[user_id].m_socket = clientSocket;
            g_clients[user_id].m_x = rand() % WORLD_WIDTH;
            g_clients[user_id].m_x = rand() % WORLD_HEIGHT;

            DWORD flags = 0;
            WSARecv(clientSocket, &g_clients[user_id].m_recv_over.wsabuf, 1, NULL,
                &flags, &g_clients[user_id].m_recv_over.over, NULL); // 여기까지 하나의 클라 소켓 등록이랑 recv 호출이 끝났음

            SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // 새로 받을 클라 소켓 
            ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept용 확장 오버랩 구조체 초기화
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
	switch (buf[1]) // 우리는 패킷을 0은 char size, 1은 char type으로 설정했으므로
	{
	case CS_PACKET_LOGIN: // case에서는 변수를 선언할때 중괄호 해줘야 변수로 선언이 된다
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
		strcpy_s(g_clients[user_id].m_name, packet->name);
		g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // 혹시 모르니까 해줬다?

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

	send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_packet(int user_id, void* packet)
{
	char* buf = reinterpret_cast<char*>(packet);

	// m_recv_over는 recv 전용 오버랩 구조체이므로 쓰면 안된다
	// 그냥 OverEx overex로 오버랩 구조체 선언해서 쓰는것도 안된다, 로컬 변수라 함수 밖으로 나가면 사라져버림 -> 할당받아야함
	OverEx* overEx = new OverEx; // 확장 오버랩 할당
	overEx->op = OP_SEND; // 타입 설정
	ZeroMemory(&overEx->over, sizeof(overEx->over)); // 오버랩 구조체 초기화
	memcpy(overEx->io_buf, buf, buf[0]); // IOCP버퍼에 패킷 내용을 패킷 크기만큼 복사
	overEx->wsabuf.buf = overEx->io_buf; // WSA버퍼에 IOCP버퍼 복사
	overEx->wsabuf.len = buf[0]; // 버퍼 사이즈 설정

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

    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}