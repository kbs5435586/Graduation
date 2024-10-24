#include "pch.h"

map<SOCKET, clientData> g_client;
playerInfo allPlayers[10];
keyInfo Key;

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void UpdatePlayer(SOCKET& sock, char* key);

void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	SOCKET sock = reinterpret_cast<SOCKET>(over->hEvent);
	if (bytes > 0)
		printf("TRACE - Send message : %d, %d (%d bytes)\n", g_client[sock].p_Info.m_position.x,
			g_client[sock].p_Info.m_position.y, bytes);
	else {

		closesocket(sock);
		return;
	}
	ZeroMemory(&g_client[sock].netData, sizeof(networkData));
	g_client[sock].netData.wsabuf.buf = g_client[sock].netData.buffer;
	g_client[sock].netData.wsabuf.len = BUF_SIZE;
	ZeroMemory(over, sizeof(*over));   //오버랩트 초기화 해주고 사용하기
	g_client[sock].netData.over.hEvent = (HANDLE)sock;

	int ret = WSARecv(sock, &g_client[sock].netData.wsabuf, 1, NULL, &flags, over, recv_complete);
}

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	SOCKET sock = reinterpret_cast<SOCKET>(over->hEvent);
	keyInfo Key{};

	if (bytes > 0)
	{
		memcpy(&Key, g_client[sock].netData.buffer, sizeof(keyInfo));
		//strcpy(&Key.key, g_client[sock].netData.buffer);
		cout << "received key : " << Key.key << endl;
		cout << "before player x,y : " << g_client[sock].p_Info.m_position.x << " , " << g_client[sock].p_Info.m_position.y << endl;
		UpdatePlayer(sock, &Key.key);
		cout << "after player x,y : " << g_client[sock].p_Info.m_position.x << " , " << g_client[sock].p_Info.m_position.y << endl;
	}
	else
	{
		closesocket(sock);
		return;
	}

	ZeroMemory(&g_client[sock].netData, sizeof(networkData));
	g_client[sock].netData.wsabuf.buf = (char*)&g_client[sock].p_Info.m_position;
	g_client[sock].netData.wsabuf.len = sizeof(Position);
	ZeroMemory(over, sizeof(*over));   //오버랩트 초기화 해주고 사용하기
	g_client[sock].netData.over.hEvent = (HANDLE)sock;
	int ret = WSASend(sock, &g_client[sock].netData.wsabuf, 1, NULL, NULL, over, send_complete);
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
		static int playerCount = 0;
		if (g_client.size() <= 10)
		{
			int addr_size = sizeof(client_addr);
			SOCKET sock = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
			if (sock == SOCKET_ERROR)
				cout << "Accept error!\n";
			else
				cout << "Accept !\n";

			int addr_size = sizeof(client_addr);
			SOCKET sock = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
			if (sock == SOCKET_ERROR)
				cout << "Accept error!\n";
			else
				cout << "Accept !\n";
			allPlayers[playerCount] = { sock,0,true };
			
			g_client[sock].p_Info.id = sock;
			g_client[sock].netData.wsabuf.buf = g_client[sock].netData.buffer;
			g_client[sock].netData.wsabuf.len = BUF_SIZE;
			DWORD flags = 0;
			ZeroMemory(&g_client[sock].netData.over, sizeof(g_client[sock].netData.over));
			g_client[sock].netData.over.hEvent = (HANDLE)sock;
			int ret = WSARecv(sock, &g_client[sock].netData.wsabuf, 1, NULL, &flags, &g_client[sock].netData.over, recv_complete);
		}
		playerCount++;
	}
	closesocket(listenSocket);
	WSACleanup();
}

void UpdatePlayer(SOCKET& sock, char* key)
{
	if (*key == 'w')
	{
		if (g_client[sock].p_Info.m_position.y <= 0)
			g_client[sock].p_Info.m_position.y = 0;
		else
			g_client[sock].p_Info.m_position.y -= 1;
	}
	if (*key == 's')
	{
		if (g_client[sock].p_Info.m_position.y >= 7)
			g_client[sock].p_Info.m_position.y = 7;
		else
			g_client[sock].p_Info.m_position.y += 1;
	}
	if (*key == 'a')
	{
		if (g_client[sock].p_Info.m_position.x <= 0)
			g_client[sock].p_Info.m_position.x = 0;
		else
			g_client[sock].p_Info.m_position.x -= 1;
	}
	if (*key == 'd')
	{
		if (g_client[sock].p_Info.m_position.x >= 7)
			g_client[sock].p_Info.m_position.x = 7;
		else
			g_client[sock].p_Info.m_position.x += 1;
	}
}