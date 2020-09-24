#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	SOCKET listenSocket;
	SOCKADDR_IN servAddr;

	// 1. 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed WSAStartup()" << endl;
		return 1;
	}

	// 2. 소켓 생성
	listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		cout << "Failed socket()" << endl;
		return 1;
	}

	// 3. 서버 자신의 주소를 셋팅
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(30002);

	if (bind(listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		cout << "Binding Error" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		cout << "Listen Error" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server Start !" << endl << endl;

	SOCKADDR_IN clientAddr;
	SOCKET clientSocket;
	int sizeClientAddr = sizeof(clientAddr);
	char sendData[255];
	char recvData[255];
	char recvByte;
	int recvCount = 0;

	// 4. 접속이 시도된 클라이언트 연결과 통신 처리
	while (1)
	{
		// 4-1 클라이언트 연결 기다리기
		clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
		if (clientSocket == INVALID_SOCKET)
			cout << "Failed accept()" << endl;
		recvCount = 0;

		cout << "accept client socket : " << clientSocket << endl;

		// 4-2 연결된 클라이언트에게 send'
		cout << "보낼 메세지 : ";
		cin >> sendData;
		send(clientSocket, sendData, strlen(sendData) + 1, 0);

		// 4-3 접속한 client가 접속을 끊을때까지 recv 대기를 반복한다.
		cout << "recv count : ";
		while ((recvByte = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
		{
			// 받은걸 그대로 보낸다
			recvData[recvByte] = NULL;
			cout << "[" << ++recvCount << "]" << (int)recvData[0] << " : " << recvData << endl;
			cout << "보낼 메세지 : ";
			cin >> sendData;
			send(clientSocket, sendData, strlen(sendData) + 1, 0);
		}
		// 4-4 접속이 끊긴 처리
		cout << endl;
		closesocket(clientSocket);
		cout << "close socket : " << clientSocket << endl;
	}

	closesocket(listenSocket);
	WSACleanup();
	return 0;
}