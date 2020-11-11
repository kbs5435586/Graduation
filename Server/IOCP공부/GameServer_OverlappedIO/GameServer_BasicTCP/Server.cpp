#include <iostream>
#include <WS2tcpip.h>
#include "protocol.h"

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define SERVER_PORT       3500

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(listenSocket, 5);

	SOCKADDR_IN client_addr;

	while (true) {
		int addr_size = sizeof(client_addr);
		SOCKET client_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);

		while (true) 
		{
			char messageBuffer[MAX_BUFFER];
			int receiveBytes = recv(client_socket, messageBuffer, MAX_BUFFER, 0);

			if (receiveBytes > 0) {
				messageBuffer[receiveBytes] = 0;
				cout << "TRACE - Receive message : " << messageBuffer << "(" << receiveBytes << " bytes)\n";
			}
			else break;

			int sendBytes = send(client_socket, messageBuffer, receiveBytes, 0);
			if (sendBytes > 0) printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
		}
		closesocket(client_socket);
	}
	closesocket(listenSocket);
	WSACleanup();
}