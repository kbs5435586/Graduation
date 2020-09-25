#include <stdio.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define SERVER_IP  "127.0.0.1"
#define SERVER_PORT       3500

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

	while (true) {
		char messageBuffer[MAX_BUFFER];
		printf("Enter Message -> ");
		scanf_s("%s", messageBuffer, MAX_BUFFER);
		int bufferLen = strlen(messageBuffer);
		int sendBytes = send(serverSocket, messageBuffer, bufferLen, 0);

		if (sendBytes > 0) {
			printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
			int receiveBytes = recv(serverSocket, messageBuffer, MAX_BUFFER, 0);
			if (receiveBytes > 0)
				printf("TRACE - Receive message : %s (%d bytes)\n", messageBuffer, receiveBytes);
		}
	}
	closesocket(serverSocket);
	WSACleanup();
}
