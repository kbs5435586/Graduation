#pragma once

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;

#pragma pack(push,1)
typedef struct Client_Info
{
	SOCKET m_socket;
	WSABUF wsabuf;
	char buffer[BUF_SIZE];
	int id;
	char key;
	int x;
	int y;
};
#pragma pack (pop)