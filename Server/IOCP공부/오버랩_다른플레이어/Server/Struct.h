#pragma once

struct Position
{
	int x = 0;
	int y = 0;
};

struct networkData
{
	WSABUF wsabuf;
	WSAOVERLAPPED over;
	char buffer[BUF_SIZE];
};

struct keyInfo
{
	char key;
};

struct clientData
{
	networkData netData;
	playerInfo p_Info;
};