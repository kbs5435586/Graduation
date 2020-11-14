#include <iostream>
#include <WS2tcpip.h>
#include "protocol.h"
#include <map>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

Client_Info Player_Data[10];

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void UpdatePlayer();

void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (bytes > 0)
		printf("TRACE - Send message : %d, %d (%d bytes)\n", Player_Data.x, Player_Data.y, bytes);
	else {
		closesocket(client_socket);
		return;
	}

	wsabuf.len = BUF_SIZE;
	ZeroMemory(over, sizeof(*over));
	int ret = WSARecv(client_socket, &wsabuf, 1, NULL, &flags, over, recv_complete);

}

void CALLBACK recv_complete(int id, DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (bytes > 0) {
		Player_Data[ID]. = 0;
		cout << "received key : " << Player_Data.key << endl;
		cout << "player x,y : " << Player_Data.x << " , " << Player_Data.y << endl;
		UpdatePlayer();
	}
	else 
	{
		closesocket(client_socket);
		return;
	}
	wsabuf.len = bytes;
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
	WSAOVERLAPPED overlapped;

	while (true)
	{
		int ID = 0;
		int addr_size = sizeof(client_addr);
		Player_Data[ID].m_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
		if (Player_Data->m_socket == SOCKET_ERROR)
			cout << "Accept error!\n";
		else
			cout << "Accept !\n";

		Player_Data[ID].wsabuf.buf = Player_Data[ID].buffer;
		Player_Data[ID].wsabuf.len = BUF_SIZE;
		DWORD flags = 0;
		ZeroMemory(&overlapped, sizeof(overlapped));

		int ret = WSARecv(Player_Data[ID].m_socket, &Player_Data[ID].wsabuf, 1, NULL, &flags, &overlapped, recv_complete);
	}
	closesocket(listenSocket);
	WSACleanup();
}

void UpdatePlayer()
{
	if (Player_Data.key == 'w')
	{
		if (Player_Data.y <= 0)
			Player_Data.y = 0;
		else
			Player_Data.y -= 1;
	}
	if (Player_Data.key == 's')
	{
		if (Player_Data.y >= 7)
			Player_Data.y = 7;
		else
			Player_Data.y += 1;
	}
	if (Player_Data.key == 'a')
	{
		if (Player_Data.x <= 0)
			Player_Data.x = 0;
		else
			Player_Data.x -= 1;
	}
	if (Player_Data.key == 'd')
	{
		if (Player_Data.x >= 7)
			Player_Data.x = 7;
		else
			Player_Data.x += 1;
	}
}