#include <iostream>
#include <WS2tcpip.h>
#include <conio.h>

using namespace std;
#pragma comment (lib,"Ws2_32.lib")

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
//constexpr char SERVER_ADDR[] = "127.0.0.1";

void PlayerMove(SOCKET* socket);
void MakeServer(SOCKET* socket);
void CloseServer(SOCKET* socket);

void err_display(const char* msg, int err_no)
{
	WCHAR* h_message;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&h_message, 0, NULL
	);
	cout << msg;
	wcout << L" 에러 => " << h_message << endl;

	while (true);
	LocalFree(h_message);
}

int main()
{
	SOCKET s_socket;

	MakeServer(&s_socket);
	PlayerMove(&s_socket);
	CloseServer(&s_socket);
}

void MakeServer(SOCKET* socket)
{
	WSADATA WSAdata;
	WSAStartup(MAKEWORD(2, 0), &WSAdata);

	*socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	SOCKADDR_IN server_a;
	memset(&server_a, 0, sizeof(server_a));
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.s_addr = INADDR_ANY;

	::bind(*socket, (sockaddr*)&server_a, sizeof(server_a));
	listen(*socket, SOMAXCONN);
}

void PlayerMove(SOCKET* socket)
{
	SOCKADDR_IN client_a;
	INT a_size = sizeof(client_a);
	SOCKET c_socket = WSAAccept(*socket, (sockaddr*)&client_a, &a_size, NULL, NULL);
	cout << "new client accepted\n";

	while (true)
	{
		char buff[BUF_SIZE + 1];

		WSABUF wsabuf;
		wsabuf.buf = buff;
		wsabuf.len = BUF_SIZE + 1;

		DWORD num_recv;
		DWORD flag = 0;
		
		WSARecv(c_socket, &wsabuf, 1, &num_recv, &flag, NULL, NULL);
		if (0 == num_recv)
			break;
		cout << "Received " << num_recv << " Bytes [" << wsabuf.buf << "]\n";
		cout << "buff[0],[1] : " << buff[0] << " , " << (int)buff[1] << endl;

		int playerPos = buff[1];

		switch (buff[0])
		{
		case 'w':
			cout << "위 입력" << endl;
			if (playerPos != 0)
				playerPos -= 10;
			break;
		case 'a':
			cout << "왼쪽 입력" << endl;
			if (playerPos != 0)
				playerPos -= 1;
			break;
		case 'd':
			cout << "오른쪽 입력" << endl;
			if (playerPos != 7)
				playerPos += 1;
			break;
		case 's':
			cout << "아래 입력" << endl;
			if (playerPos != 70)
				playerPos += 10;
			break;
		}

		buff[1]=playerPos;

		DWORD num_sent;
		wsabuf.len = BUF_SIZE + 1;
		int ret = WSASend(c_socket, &wsabuf, 1, &num_sent, 0, NULL, NULL);
		if (SOCKET_ERROR == ret) {
			err_display("Error Disconnect", WSAGetLastError());
		}
		cout << "Sent " << wsabuf.len << " Bytes, 좌표 : [" << (int)buff[1] << "]\n";
	}
	cout << "Client Connection Close" << endl;
	closesocket(c_socket);
}

void CloseServer(SOCKET* socket)
{
	closesocket(*socket);
	WSACleanup();
}
