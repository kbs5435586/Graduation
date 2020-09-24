#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <time.h>

#pragma comment (lib,"ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	// 1.윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed Startup()" << endl;
		return 1;
	}

	// 2. 소켓 생성
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hSocket == INVALID_SOCKET)
	{
		cout << "Failed Socket()" << endl;
		WSACleanup();
	}

	// 3. 서버 연결
	// 3-1. 연결할 서버 주소 셋팅
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
	servAddr.sin_port = htons(30002);

	// 3-2. 서버에 연결 시도
	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		cout << "Failed connect()" << endl;
		closesocket(hSocket);
		WSACleanup();
		return 1;
	}

	// 4. 서버로부터 통신 대기
	int recvByte;
	char recvData[255];
	char sendData[255];

	recvByte = recv(hSocket, recvData, sizeof(recvData), 0);
	if (recvByte == SOCKET_ERROR)
		cout << "Failed recv()" << endl;

	cout << "recv : " << recvData << endl;

	// 5. 연결된 서버와 가위바위보 진행
	char resultRock[4][4] = {
		{0,0,0,0},
		{0,0,-1,1},
		{0,1,0,-1},
		{0,-1,1,0}
	};

	char result = 0;
	char itemClient = 0;
	char itemServer = 0;
	int flow = 5;

	srand((unsigned int)time(NULL));

	while (flow--)
	{
		cout << "당신의 선택은? : ";
		cin >> itemClient;
		if (itemClient > 0 && itemClient < 4)
		{
			sendData[0] = itemClient;
			// 자신이 낸 가바보를 서버로 보냄
			send(hSocket, sendData, 1, 0);
		}

		// server의 가바보 정보 recv 대기
		recvByte = recv(hSocket, recvData, sizeof(recvData), 0);
		if (recvByte < 1)
			break;
		itemServer = recvData[0];
		result = resultRock[itemClient][itemServer];

		// 결과 화면 출력
		
		Sleep(2000);
		sendData[0] = 1;
		// server의 값을 client로 전송 처리
		send(hSocket, sendData, 1, 0);
	}

	// 5. 소켓 종료 -> 윈속 종료
	closesocket(hSocket);
	WSACleanup();

	system("pause");
	return 0;
}
