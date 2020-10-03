#pragma comment (lib,"Ws2_32.lib")

#include <iostream>
#include <WS2tcpip.h>
#include <conio.h>

using namespace std;

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;

#define BOARDWIDTH 8
#define BOARDHEIGHT 8

const char* board[BOARDWIDTH][BOARDHEIGHT];

void DrawMap(int x, int y);
void CreateMap();

int main()
{
	WSADATA WSAdata;
	WSAStartup(MAKEWORD(2, 2), &WSAdata);

	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN server_a;
	memset(&server_a, 0, sizeof(server_a));
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);

	while (true)
	{
		cout << "IP 주소를 입력하세요 : ";
		char SERVER_ADDR[20];
		cin.getline(SERVER_ADDR, 20);
		inet_pton(AF_INET, SERVER_ADDR, &server_a.sin_addr);

		if (WSAConnect(s_socket, (sockaddr*)&server_a, sizeof(server_a), NULL, NULL, NULL, NULL) == 0)
		{
			cout << "Connect !" << endl;
			break;
		}
		else
			cout << "Fail !" << endl;
	}

	int playerPos = 0, playerPosX = 0, playerPosY = 0;
	CreateMap();

	while (true)
	{
		cout << "wasd 또는 방향키를 입력하세요 : ";
		char buff[BUF_SIZE + 1];
		buff[0] = NULL;
		buff[0] = _getch();
		buff[1] = playerPos;
		buff[2] = NULL;

		cout << "buff[0],[1] : " << (int)buff[0] << " , " << (int)buff[1] << endl;

		if (GetAsyncKeyState(VK_UP) & 0x0001)
		{
			buff[0] = 'w';
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x0001)
		{
			buff[0] = 'a';
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x0001)
		{
			buff[0] = 's';
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
		{
			buff[0] = 'd';
		}

		WSABUF wsabuf;
		wsabuf.buf = buff;
		wsabuf.len = static_cast<ULONG>(strlen(buff) + 1);

		if (0 == wsabuf.len)
			break;
		DWORD num_sent;

		WSASend(s_socket, &wsabuf, 1, &num_sent, 0, NULL, NULL);

		cout << "Sent " << wsabuf.len << " Bytes [" << buff[0] << (int)wsabuf.buf[1] << "]" << endl;
		cout << "size[0],[1] : " << sizeof(buff[0]) << " , " << sizeof(wsabuf.buf[1]) << endl;

		DWORD num_recv;
		DWORD flag = 0;
		wsabuf.len = BUF_SIZE;
		WSARecv(s_socket, &wsabuf, 1, &num_recv, &flag, NULL, NULL);

		playerPos = buff[1];
		playerPosX = buff[1] / 10;
		playerPosY = buff[1] % 10;

		cout << "Received " << num_recv << " Bytes , 좌표 : [" << playerPosX << " , " << playerPosY << "]" << endl;
		cout << "pos x,y : " << playerPosX << " , " << playerPosY << endl;

		DrawMap(playerPosX, playerPosY);
	}
	closesocket(s_socket);
	WSACleanup();
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
	cout << endl;
}