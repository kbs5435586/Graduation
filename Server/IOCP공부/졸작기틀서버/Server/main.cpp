#include "pch.h"

clientData g_client[MAX_PLAYER];

SOCKET listenSocket;
HANDLE iocp;
OVER_EX g_accept_over;

void send_packet(int id, void* p)
{
	unsigned char* packet = reinterpret_cast<unsigned char*>(p);
	OVER_EX* send_over = new OVER_EX;
	memcpy(send_over->iocp_buf, packet, packet[0]);
	send_over->op_mode = OP_MODE_SEND;
	send_over->wsa_buf.buf = reinterpret_cast<CHAR*>(send_over->iocp_buf);
	send_over->wsa_buf.len = packet[0];
	ZeroMemory(&send_over->wsa_over, sizeof(send_over->wsa_over));
	WSASend(g_client[id].m_sock, &send_over->wsa_buf, 1, NULL, 0, &send_over->wsa_over, NULL);
}

void send_login_ok(int id)
{
	StoC_packet_login_ok p;
	p.exp = 0;
	p.hp = 100;
	p.id = id;
	p.level = 1;
	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	p.x = g_client[id].x;
	p.y = g_client[id].y;
	send_packet(id, &p);
}

void send_move_packet(int id)
{
	StoC_packet_move p;
	p.id = id;
	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE;
	p.x = g_client[id].x;
	p.y = g_client[id].y;
	send_packet(id, &p);
}

void process_move(int id, char dir)
{
	short y = g_client[id].y;
	short x = g_client[id].x;
	switch (dir) {
	case MV_UP:
		if (y > 0)
			y--;
		break;
	case MV_DOWN:
		if (y < WORLD_HEIGHT - 1)
			y++;
		break;
	case MV_LEFT:
		if (x > 0)
			x--;
		break;
	case MV_RIGHT:
		if (x < WORLD_WIDTH - 1)
			x++;
		break;
	default:
		cout << "Unknown Direction in CS_MOVE packet.\n";
		while (true);
	}
	g_client[id].x = x;
	g_client[id].y = y;

	send_move_packet(id);
}

void process_packet(int id) // 데이터가 날라 왔을때 해당 패킷을 처리하는 패킷 처리 루틴
{
	char p_type = g_client[id].m_packet_start[1]; // 패킷이 여기있음
	// p_type 은 패킷 타입
	switch (p_type)
	{
	case CS_LOGIN: // 로그인 패킷일 경우
	{
		CtoS_packet_login* p = reinterpret_cast<CtoS_packet_login*>(g_client[id].m_packet_start);
		strcpy_s(g_client[id].name, p->name);
		send_login_ok(id);
	}
	break;
	case CS_MOVE: // move 패킷일 경우
	{
		CtoS_packet_move* p1 = reinterpret_cast<CtoS_packet_move*>(g_client[id].m_packet_start);
		process_move(id, p1->direction);
	}
	break;
	default:
		cout << "Unknown Packet Type [" << p_type << "] from Client [" << id << "]\n";
		while (true);
	}
}

void process_recv(int id, DWORD iosize)
{
	unsigned char p_size = g_client[id].m_packet_start[0];
	unsigned char* next_recv_ptr = g_client[id].m_recv_start + iosize;
	while (p_size <= next_recv_ptr - g_client[id].m_packet_start) {
		process_packet(id);
		g_client[id].m_packet_start += p_size;
		if (g_client[id].m_packet_start < next_recv_ptr)
			p_size = g_client[id].m_packet_start[0];
		else break;
	}

	int left_data = next_recv_ptr - g_client[id].m_packet_start;

	if ((MAX_BUFFER - (next_recv_ptr - g_client[id].m_recv_over.iocp_buf)) < MIN_BUFF_SIZE)
	{
		memcpy(g_client[id].m_recv_over.iocp_buf,
			g_client[id].m_packet_start, left_data);
		g_client[id].m_packet_start = g_client[id].m_recv_over.iocp_buf;
		g_client[id].m_recv_start = g_client[id].m_packet_start + left_data;
	}
	DWORD recv_flag = 0;
	WSARecv(g_client[id].m_sock, &g_client[id].m_recv_over.wsa_buf,
		1, NULL, &recv_flag, &g_client[id].m_recv_over.wsa_over, NULL);
}

void add_new_client(SOCKET ns)
{
	int i;
	for (i = 0; i < MAX_PLAYER; i++)
	{
		if (false == g_client[i].in_use)
			break;
	} 

	if (MAX_PLAYER == i)
	{
		cout << "Max User!!\n";
		closesocket(ns);
	}
	else
	{
		g_client[i].id = i;
		g_client[i].in_use = true;

		g_client[i].m_packet_start = g_client[i].m_recv_over.iocp_buf;

		g_client[i].m_recv_over.op_mode = OP_MODE_RECV; // 새클라 받으면 오버랩 모드를 리시브로 변경
		g_client[i].m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(g_client[i].m_recv_over.iocp_buf); // iocp 버퍼를 일반버퍼에 복사
		g_client[i].m_recv_over.wsa_buf.len = sizeof(g_client[i].m_recv_over.iocp_buf); // 일반버퍼 크기도 리시브 버퍼 크기로
		ZeroMemory(&g_client[i].m_recv_over.wsa_over, sizeof(g_client[i].m_recv_over.wsa_over)); // 그러고 리시브 오버랩 초기화

		g_client[i].m_recv_start = g_client[i].m_recv_over.iocp_buf;
		g_client[i].m_sock = ns;
		g_client[i].name[0] = 0;
		g_client[i].x = rand() % WORLD_WIDTH;
		g_client[i].y = rand() % WORLD_HEIGHT;

		DWORD flags = 0;
		WSARecv(g_client[i].m_sock, &g_client[i].m_recv_over.wsa_buf, 1, NULL, &flags, &g_client[i].m_recv_over.wsa_over, NULL);
	}

	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = sock;
	ZeroMemory(&g_accept_over.wsa_over, 0, sizeof(g_accept_over.wsa_over));
	AcceptEx(listenSocket, sock, g_accept_over.iocp_buf, 0, 16, 16, NULL, &g_accept_over.wsa_over);
}

// 1단계 일반 tcp ip 소켓통신 -> 2. 동기화에서 비동기화로 바꾸는 Overlapped
// Overlapped 구조 단점들을 보완해 추가한 
int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort((HANDLE)listenSocket, iocp, KEY_SERVER, 0); // IOCP 객체 생성 + Socket을 IOCP에 연결

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(listenSocket, 5);

	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = sock;
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));

	AcceptEx(listenSocket, sock, g_accept_over.iocp_buf, 0, 16, 16, NULL, &g_accept_over.wsa_over);

	while (true)
	{
		DWORD io_size;
		ULONG_PTR key;
		WSAOVERLAPPED* lpOver;

		int ret = GetQueuedCompletionStatus(iocp, &io_size, &key, &lpOver, INFINITE);

		OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(lpOver);

		switch (over_ex->op_mode)
		{
		case OP_MODE_ACCEPT:
			add_new_client(static_cast<SOCKET>(over_ex->wsa_buf.len));
			break;
		case OP_MODE_RECV:
			process_recv(key, io_size);
			break;
		case OP_MODE_SEND:
			delete over_ex; // send할때 사용한 오버를 반환, 릴리스 해야함
			break;
		}
	}

	closesocket(listenSocket);
	WSACleanup();
}