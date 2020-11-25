#include "pch.h"

clientData g_client[MAX_PLAYER];

SOCKET listenSocket;
HANDLE iocp;
OVER_EX g_accept_over;

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
	CreateIoCompletionPort((HANDLE)listenSocket, iocp, KEY_SERVER, 0);

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
			add_new_client
			break;
		case OP_MODE_RECV:
			break;
		case OP_MODE_SEND:
			break;
		}
	}

	closesocket(listenSocket);
	WSACleanup();
}