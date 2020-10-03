#include <iostream>
#include <conio.h>
#include "protocal.h"

using namespace std;

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
void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);

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

	*socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_a;
	memset(&server_a, 0, sizeof(server_a));
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.s_addr = INADDR_ANY;

	::bind(*socket, (sockaddr*)&server_a, sizeof(server_a));
	listen(*socket, MAX_PLAYER);
}

void PlayerMove(SOCKET* socket)
{
	WSAOVERLAPPED overlapped;

	SOCKADDR_IN client_a;

	while (true)
	{
		INT a_size = sizeof(client_a);
		c_socket = accept(*socket, (sockaddr*)&client_a, &a_size);
		cout << "new client accepted\n";
		cout << "Client Addr = " << (sockaddr*)&client_a << endl;

		wsabuf.buf = buff;
		wsabuf.len = BUF_SIZE;
		DWORD flag = 0;
		ZeroMemory(&overlapped, sizeof(overlapped));

		WSARecv(c_socket, &wsabuf, 1, NULL, &flag, &overlapped, recv_complete);
	}
}

void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (bytes > 0)
		cout << "Sent " << bytes << " Bytes, 좌표 : [" << (int)buff[1] << "]" << endl << endl;
	else {
		closesocket(c_socket);
		return;
	}

	wsabuf.len = BUF_SIZE;
	ZeroMemory(over, sizeof(*over));
	int ret = WSARecv(c_socket, &wsabuf, 1, NULL, &flags, over, recv_complete);
}

void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (bytes > 0) {
		buff[bytes] = 0;
		cout << "Received " << bytes << " Bytes [" << buff << "]\n";
		cout << "buff[0],[1] : " << buff[0] << " , " << (int)buff[1] << endl;

		int playerPos = buff[1];

		switch (buff[0])
		{
		case '&':
			cout << "위 입력" << endl;
			if (playerPos != 0)
				playerPos -= 10;
			break;
		case '%':
			cout << "왼쪽 입력" << endl;
			if ((playerPos % 10) != 0)
				playerPos -= 1;
			break;
		case ' ':
			cout << "오른쪽 입력" << endl;
			if ((playerPos % 10) != 7)
				playerPos += 1;
			break;
		case '(':
			cout << "아래 입력" << endl;
			if ((playerPos / 10) != 7)
				playerPos += 10;
			break;
		}

		buff[0] = 0;
		buff[1] = playerPos;
	}
	else {
		cout << "Client Connection Close" << endl;
		closesocket(c_socket);
		return;
	}

	wsabuf.len = bytes;
	ZeroMemory(over, sizeof(*over));   //오버랩트 초기화 해주고 사용하기
	int ret = WSASend(c_socket, &wsabuf, 1, NULL, NULL, over, send_complete);
	if (SOCKET_ERROR == ret) {
		err_display("Error Disconnect", WSAGetLastError()); // 여기서 걸림
	}
}

void CloseServer(SOCKET* socket)
{
	closesocket(*socket);
	WSACleanup();
}
