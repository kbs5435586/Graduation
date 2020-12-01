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

void process_packet(int id) // �����Ͱ� ���� ������ �ش� ��Ŷ�� ó���ϴ� ��Ŷ ó�� ��ƾ
{
	char p_type = g_client[id].m_packet_start[1]; // ��Ŷ�� ��������
	// p_type �� ��Ŷ Ÿ��
	switch (p_type)
	{
	case CS_LOGIN: // �α��� ��Ŷ�� ���
	{
		CtoS_packet_login* p = reinterpret_cast<CtoS_packet_login*>(g_client[id].m_packet_start);
		strcpy_s(g_client[id].name, p->name);
		send_login_ok(id);
	}
	break;
	case CS_MOVE: // move ��Ŷ�� ���
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

		g_client[i].m_recv_over.op_mode = OP_MODE_RECV; // ��Ŭ�� ������ ������ ��带 ���ú�� ����
		g_client[i].m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(g_client[i].m_recv_over.iocp_buf); // iocp ���۸� �Ϲݹ��ۿ� ����
		g_client[i].m_recv_over.wsa_buf.len = sizeof(g_client[i].m_recv_over.iocp_buf); // �Ϲݹ��� ũ�⵵ ���ú� ���� ũ���
		ZeroMemory(&g_client[i].m_recv_over.wsa_over, sizeof(g_client[i].m_recv_over.wsa_over)); // �׷��� ���ú� ������ �ʱ�ȭ

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

// 1�ܰ� �Ϲ� tcp ip ������� -> 2. ����ȭ���� �񵿱�ȭ�� �ٲٴ� Overlapped
// Overlapped ���� �������� ������ �߰��� 
int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort((HANDLE)listenSocket, iocp, KEY_SERVER, 0); // IOCP ��ü ���� + Socket�� IOCP�� ����

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
			delete over_ex; // send�Ҷ� ����� ������ ��ȯ, ������ �ؾ���
			break;
		}
	}

	closesocket(listenSocket);
	WSACleanup();
}