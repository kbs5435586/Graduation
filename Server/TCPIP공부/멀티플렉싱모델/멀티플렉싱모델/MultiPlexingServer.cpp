#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib,"ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	SOCKET sockServer;
	SOCKADDR_IN addrServer;

	// 1. 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed Startup()" << endl;
		return 1;
	}

	// 2. server socket 생성
	sockServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockServer == INVALID_SOCKET)
	{
		WSACleanup();
		cout << "Failed Server Socket()" << endl;
		return 1;
	}

	// 3. server 주소 binding 및 listen socket
	memset(&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = AF_INET;
	//addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "127.0.0.1", &addrServer.sin_addr);
	addrServer.sin_port = htons(30002);

	if (bind(sockServer, (SOCKADDR*)&addrServer, sizeof(addrServer)))
	{
		cout << "bind Error" << endl;
		closesocket(sockServer);
		WSACleanup();
		return 1;
	}

	if (listen(sockServer, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "listen Error" << endl;
		closesocket(sockServer);
		WSACleanup();
		return 1;
	}

	cout << "START SERVER !!" << endl;

	// 4★★★★★. select 함수 셋팅

	/*
	FD_ZERO() : fd_set 구조체의 fd_count를 0으로 만들어주는 매크로 함수.
	FD_SET() : fd_set 구조체에 감시 대상을 넣어주는 매크로 함수.
	FD_ISSET() : fd_set 구조체에 해당 감시 대상이 있는지 알려주는 매크로 함수.
	FD_CLR() : 	fd_set 구조체에서 해당 감시 대상을 제거하는 매크로 함수.
	*/

	fd_set fdSocketInfos; // socket 정보를 저장하고 있는 구조체
	fd_set fdReads;		  // 변화를 읽어올 구조체

	int iRet;
	unsigned int i;
	SOCKET sockClient, sockTemp;
	SOCKADDR_IN addrClient;
	int addrSize;
	char recvBuffer[255];
	int recvSize;

	FD_ZERO(&fdSocketInfos);
	FD_SET(sockServer, &fdSocketInfos);

	while (true)
	{
		fdReads = fdSocketInfos;
		// 블로킹 - 감시할 소켓을 넣고 대기
		iRet = select(0, &fdReads, 0, 0, 0);
		if (iRet == SOCKET_ERROR)
			break;

		for (i = 0; i < fdSocketInfos.fd_count; i++)
		{
			sockTemp = fdSocketInfos.fd_array[i];

			// 보유한 소켓중에 변화가 일어난 소켓이 있는지
			if (FD_ISSET(sockTemp, &fdReads))
			{
				// Reads가 일어난 소켓이 서버인 경우 accept
				if (sockTemp == sockServer)
				{
					addrSize = sizeof(addrClient);
					sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &addrSize);
					FD_SET(sockClient, &fdSocketInfos);
					
					cout << i << " connected " << sockClient << "  " << addrClient.sin_addr.S_un.S_un_b.s_b1
						<< " , " << addrClient.sin_addr.S_un.S_un_b.s_b2 << " , " << addrClient.sin_addr.S_un.S_un_b.s_b3
						<< " , " << addrClient.sin_addr.S_un.S_un_b.s_b4 << " : " << addrClient.sin_port << endl;
				}
				else // client 인 경우 recv 처리
				{
					recvSize = recv(sockTemp, recvBuffer, sizeof(recvBuffer), 0);
					cout << i << " : socket = " << sockTemp << " recv" << endl;
					if (recvSize > 0)
						send(sockTemp, recvBuffer, recvSize, 0);
					else {
						FD_CLR(sockTemp, &fdSocketInfos);
						closesocket(sockTemp);
						cout << "close socket : " << sockTemp << endl;
					}
				}
			}
		}
	}
	closesocket(sockServer);
	WSACleanup();

	return 0;
}