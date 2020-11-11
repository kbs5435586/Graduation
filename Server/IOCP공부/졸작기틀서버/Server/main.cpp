#include <iostream>
#include <WS2tcpip.h>
#include "protocol.h"

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;

Client_Info Player_Data;

void UpdatePlayer();

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

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
		SOCKET client_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
		if (client_socket == SOCKET_ERROR)
			cout << "Accept error!\n";
		else
			cout << "Accept !\n";


		while (true)
		{
			int receiveBytes = recv(client_socket, (char*)&Player_Data, sizeof(Client_Info), 0);
			if (receiveBytes > 0)
			{
				cout << "received key : " << Player_Data.key << endl;
				cout << "player x,y : " << Player_Data.x << " , " << Player_Data.y << endl;
			}
			else break;

			UpdatePlayer();

			int sendBytes = send(client_socket, (char*)&Player_Data, sizeof(Client_Info), 0);
			if (sendBytes > 0) printf("TRACE - Send message : %d bytes)\n", sendBytes);
		}
		closesocket(client_socket);
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