#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

#pragma comment (lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	SOCKET Socket;
	SOCKADDR_IN serverAddress;

	// 1. 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		cout << "Failed WSAStartup()" << endl;

	// 2. 소켓 생성
	Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // SOCK_STREAM : 순차적이고 신뢰할 수 있는 네트워크
	if (Socket == INVALID_SOCKET)
		cout << "Failed socket()" << endl;

	// 3. 서버 연결
	// 3.1 연결할 서버 주소 셋팅
	memset(&serverAddress, 0, sizeof(serverAddress)); // 까먹기 쉬움 ★★★★★
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr); // 127.0.0.1 : 로컬호스트, 자신의 컴퓨터 
															  // 로컬 컴퓨터를 원격 컴퓨터인것 처럼 통신할 수 있어 테스트 목적으로 주로 사용
	serverAddress.sin_port = htons(30002); // 호스트 투 네트웤 숏 : 전송 시 상대방의 네트워크 표준에 맞춰주는 역할
	// 이 과정을 통해 serverAddress 구조체 안에 주소 체계, IP, 포트 설정


	// 3.2 서버에 연결 시도
	if (connect(Socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) // connect (이 소켓을 이용해서, 이 주소로 연결을 해라, 주소의 크기)
		cout << "Failed connect()" << endl;

	// 4. 서버로부터 통신 대기
	int receiveAddrSize;
	char receiveData[255];

	receiveAddrSize = recv(Socket, receiveData, sizeof(receiveData), 0); // recv(어떠한 소켓으로 받겠다, 받을 버퍼의 크기, )
	if (receiveAddrSize == -1) // 연결 끊겼을 시 -1 반환, 연결 되있을 시 recv 크기 반환
		cout << "Failed recv()" << endl;

	// connect, recv 둘다 블로킹 함수(동기 함수)
	// 그 함수가 실행이 되는 동안은 이후 코드로 진행이 안됨

	cout << "recv " << receiveAddrSize << " message : " << receiveData << endl;

	// 5. 소켓 종료 -> 윈속 종료
	closesocket(Socket); // 소켓 생성이랑 같이
	WSACleanup(); // 윈속 초기화랑 같이

	system("pause");
	return 0;
}



/*
버퍼(Buffer)란?

하나의 장치에서 다른 장치로 데이터를 전송할 경우에 양자간의 데이터의 전송속도나 
처리속도의 차를 보상하여 양호하게 결합할 목적으로 사용하는 기억영역을 버퍼 또는 버퍼 에어리어라고 한다. 
보통 중앙처리장치와 단말이나 다른 입출력장치사이의 데이터 송수신에는 입출력 영역으로서 버퍼를 필요로 한다
*/