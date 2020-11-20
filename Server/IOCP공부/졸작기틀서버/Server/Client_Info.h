#pragma once
class Client_Info
{
public:
	Client_Info();
	~Client_Info();

public:
	void Init();
	void Update();

public:
	SOCKET m_sock;
	WSABUF wsabuf;
	WSAOVERLAPPED over;
	send_player_packet* s_packet;
	recv_player_packet* s_packet;
	char buffer[BUF_SIZE];

public:
	Position m_position;
	char key;
};

