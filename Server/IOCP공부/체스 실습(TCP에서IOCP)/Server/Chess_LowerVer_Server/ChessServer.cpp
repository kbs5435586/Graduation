#include <iostream>
#include <WS2tcpip.h>
#include <conio.h>

using namespace std;
#pragma comment (lib,"Ws2_32.lib")

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
//constexpr char SERVER_ADDR[] = "127.0.0.1";

void CreateMap();
void DrawMap(int x, int y);
void PlayerMove(SOCKET* socket, int* x, int* y);
void MakeServer(SOCKET* socket);
void CloseServer(SOCKET* socket);

#define BOARDWIDTH 8
#define BOARDHEIGHT 8

const char* board[BOARDWIDTH][BOARDHEIGHT];

int main()
{
	int playerPosX = 0, playerPosY = 0;
	SOCKET s_socket;

	CreateMap();
	MakeServer(&s_socket);

	PlayerMove(&s_socket, &playerPosX, &playerPosY);

	CloseServer(&s_socket);
}

void CreateMap()
{
	const char* white = "□";
	const char* black = "■";

	for (int i = 0; i < BOARDWIDTH; ++i)
	{
		for (int j = 0; j < BOARDHEIGHT; ++j)
		{
			if (i % 2 == 0)
			{
				if (j % 2 == 0)
					board[i][j] = white;
				else
					board[i][j] = black;
			}
			else
			{
				if (j % 2 == 0)
					board[i][j] = black;
				else
					board[i][j] = white;
			}
		}
	}
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

void DrawMap(int x, int y)
{
	for (int i = 0; i < BOARDWIDTH; ++i)
	{
		for (int j = 0; j < BOARDHEIGHT; ++j)
		{
			if (i == x && j == y)
				cout << "☆";
			else
				cout << board[i][j];
		}
		cout << endl;
	}
}

void PlayerMove(SOCKET* socket, int* x, int* y)
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
		wsabuf.len = BUF_SIZE;

		DWORD num_recv;
		DWORD flag = 0;
		WSARecv(c_socket, &wsabuf, 1, &num_recv, &flag, NULL, NULL);
		if (0 == num_recv) 
			break;
		cout << "Received " << num_recv << " Bytes [" << wsabuf.buf << "]\n";

		switch (buff[0])
		{
		case 'w':
			cout << "위 입력" << endl;
			if (*x != 0)
				*x -= 1;
			break;
		case 'a':
			cout << "왼쪽 입력" << endl;
			if (*y != 0)
				*y -= 1;
			break;
		case 'd':
			cout << "오른쪽 입력" << endl;
			if (*y != 7)
				*y += 1;
			break;
		case 's':
			cout << "아래 입력" << endl;
			if (*x != 7)
				*x += 1;
			break;
		}

		buff[0] = *x;
		buff[1] = *y;

		DWORD num_sent;
		wsabuf.len = num_recv;
		WSASend(c_socket, &wsabuf, 1, &num_sent, 0, NULL, NULL);
		cout << "Sent " << wsabuf.len << " Bytes, 좌표 : [" << (int)buff[0] << ", " << (int)buff[1] << "]\n";

		DrawMap(*x, *y);
	}
	cout << "Client Connection Close" << endl;
	closesocket(c_socket);
}

void CloseServer(SOCKET *socket)
{
	closesocket(*socket);
	WSACleanup();
}
