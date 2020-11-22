#include "pch.h"

map<SOCKET, clientData> g_client;

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void UpdatePlayer(char key);

void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	SOCKET sock = reinterpret_cast<SOCKET>(over->hEvent);
	if (bytes > 0)
		printf("TRACE - Send message : %d, %d (%d bytes)\n", g_client(ID).m_position.x, Player_Data[ID].m_position.y, bytes);
	else {

		closesocket(Player_Data[ID].m_sock);
		return;
	}
	Player_Data[ID].wsabuf.buf = Player_Data[ID].buffer;
	Player_Data[ID].wsabuf.len = sizeof(recv_player_packet);
	ZeroMemory(over, sizeof(*over));
	int ret = WSARecv(Player_Data[ID].m_sock, &Player_Data[ID].wsabuf, 1, NULL, &flags, over, recv_complete);

}

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	SOCKET sock = reinterpret_cast<SOCKET>(over->hEvent);
	if (bytes > 0) {
		g_client[sock] .buffer[bytes] = 0;
		strcpy(&Player_Data[ID].key, Player_Data[ID].wsabuf.buf);
		cout << "received key : " << Player_Data[ID].key << endl;
		cout << "before player x,y : " << Player_Data[ID].m_position.x << " , " << Player_Data[ID].m_position.y << endl;
		UpdatePlayer(Player_Data[ID].key);
		cout << "after player x,y : " << Player_Data[ID].m_position.x << " , " << Player_Data[ID].m_position.y << endl;
	}
	else 
	{
		closesocket(Player_Data[ID].m_sock);
		return;
	}
	Player_Data[ID].wsabuf.buf = (char*)&Player_Data[ID].m_position;
	Player_Data[ID].wsabuf.len = sizeof(send_player_packet);
	ZeroMemory(over, sizeof(*over));   //오버랩트 초기화 해주고 사용하기
	int ret = WSASend(Player_Data[ID].m_sock, &Player_Data[ID].wsabuf, 1, NULL, NULL, over, send_complete);
}
int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(listenSocket, SOMAXCONN);

	SOCKADDR_IN client_addr;

	while (true)
	{
		int addr_size = sizeof(client_addr);
		SOCKET sock = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
		if (sock == SOCKET_ERROR)
			cout << "Accept error!\n";
		else
			cout << "Accept !\n";

		g_client[sock].netData.m_sock = sock;
		g_client[sock].netData.wsabuf.buf = g_client[sock].p_Info.buffer;
		g_client[sock].netData.wsabuf.len = BUF_SIZE;
		DWORD flags = 0;
		ZeroMemory(&g_client[sock].netData.over, sizeof(g_client[sock].netData.over));

		int ret = WSARecv(g_client[sock].netData.m_sock, &g_client[sock].netData.wsabuf, 
			1, NULL, &flags, &g_client[sock].netData.over, recv_complete);
	}
	closesocket(listenSocket);
	WSACleanup();
}

void UpdatePlayer(char key)
{
	for (int i = 0; i < ID; i++)
	{
		if (key == 'w')
		{
			if (Player_Data[i].m_position.y <= 0)
				Player_Data[i].m_position.y = 0;
			else
				Player_Data[i].m_position.y -= 1;
		}
		if (key == 's')
		{
			if (Player_Data[i].m_position.y >= 7)
				Player_Data[i].m_position.y = 7;
			else
				Player_Data[i].m_position.y += 1;
		}
		if (key == 'a')
		{
			if (Player_Data[i].m_position.x <= 0)
				Player_Data[i].m_position.x = 0;
			else
				Player_Data[i].m_position.x -= 1;
		}
		if (key == 'd')
		{
			if (Player_Data[i].m_position.x >= 7)
				Player_Data[i].m_position.x = 7;
			else
				Player_Data[i].m_position.x += 1;
		}
	}
}