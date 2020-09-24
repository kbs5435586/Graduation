#include <iostream>
#include <WS2tcpip.h>
#include <conio.h>

using namespace std;
#pragma comment (lib,"Ws2_32.lib")

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;

int main()
{
	WSADATA WSAdata;
	WSAStartup(MAKEWORD(2, 0), &WSAdata);

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


	while (true)
	{
		cout << "wasd 또는 방향키를 입력하세요 : ";
		char buff[BUF_SIZE + 1];
		buff[0] = NULL;
		buff[1] = NULL;

		buff[0] = _getch();

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
		
		if (1 == wsabuf.len)
			break;
		DWORD num_sent;

		WSASend(s_socket, &wsabuf, 1, &num_sent, 0, NULL, NULL);
		cout << "Sent "<<wsabuf.len <<" Bytes [" << buff << "]" << endl;
		DWORD num_recv;
		DWORD flag = 0;
		wsabuf.len = BUF_SIZE;
		WSARecv(s_socket, &wsabuf, 1, &num_recv, &flag, NULL, NULL);
		cout << "Received " << num_recv << " Bytes , 좌표 : [" << (int)wsabuf.buf[0] << ", " << (int)wsabuf.buf[1] << "]" << endl;
	}
	closesocket(s_socket);
	WSACleanup();
}