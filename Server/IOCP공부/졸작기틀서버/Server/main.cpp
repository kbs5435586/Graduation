#include "pch.h"
#include "Client_Info.h"

int ID = 0;
Client_Info Player_Data[MAX_PLAYER];

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void UpdatePlayer();

void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (bytes > 0)
		printf("TRACE - Send message : %d, %d (%d bytes)\n", Player_Data.x, Player_Data.y, bytes);
	else {
		closesocket(Player_Data[ID].m_sock);
		return;
	}

	wsabuf.len = BUF_SIZE;
	ZeroMemory(over, sizeof(*over));
	int ret = WSARecv(client_socket, &wsabuf, 1, NULL, &flags, over, recv_complete);

}

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (bytes > 0) {
		Player_Data[ID].buffer[bytes] = 0;
		strcpy(&Player_Data[ID].key, Player_Data[ID].buffer);
		cout << "received key : " << Player_Data.key << endl;
		cout << "player x,y : " << Player_Data.x << " , " << Player_Data.y << endl;
		UpdatePlayer();
	}
	else 
	{
		closesocket(client_socket);
		return;
	}
	Player_Data[ID].wsabuf.len = bytes;
	ZeroMemory(over, sizeof(*over));   //오버랩트 초기화 해주고 사용하기
	int ret = WSASend(client_socket, &wsabuf, 1, NULL, NULL, over, send_complete);

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
		Player_Data[ID].m_sock = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
		if (Player_Data[ID].m_sock == SOCKET_ERROR)
			cout << "Accept error!\n";
		else
			cout << "Accept !\n";

		Player_Data[ID].wsabuf.buf = Player_Data[ID].buffer;
		Player_Data[ID].wsabuf.len = BUF_SIZE;
		DWORD flags = 0;
		ZeroMemory(&Player_Data[ID].over, sizeof(Player_Data[ID].over));

		int ret = WSARecv(Player_Data[ID].m_sock, &Player_Data[ID].wsabuf, 1, NULL, &flags, &Player_Data[ID].over, recv_complete);
		ID++;
	}
	closesocket(listenSocket);
	WSACleanup();
}

void UpdatePlayer()
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (Player_Data[i].packet->key == 'w')
		{
			if (Player_Data[i].packet->y <= 0)
				Player_Data[i].packet->y = 0;
			else
				Player_Data[i].packet->y -= 1;
		}
		if (Player_Data[i].packet->key == 's')
		{
			if (Player_Data[i].packet->y >= 7)
				Player_Data[i].packet->y = 7;
			else
				Player_Data[i].packet->y += 1;
		}
		if (Player_Data[i].packet->key == 'a')
		{
			if (Player_Data[i].packet->x <= 0)
				Player_Data[i].packet->x = 0;
			else
				Player_Data[i].packet->x -= 1;
		}
		if (Player_Data[i].packet->key == 'd')
		{
			if (Player_Data[i].packet->x >= 7)
				Player_Data[i].packet->x = 7;
			else
				Player_Data[i].packet->x += 1;
		}
	}
}