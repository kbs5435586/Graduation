#pragma once

struct Position
{
	int x = 0;
	int y = 0;
};

struct networkData
{
	SOCKET m_sock;
	WSABUF wsabuf;
	WSAOVERLAPPED over;
};

struct playerInfo
{
	char buffer[BUF_SIZE];
	Position m_position;
	char key;
};

struct clientData
{
	networkData netData;
	playerInfo p_Info;
};