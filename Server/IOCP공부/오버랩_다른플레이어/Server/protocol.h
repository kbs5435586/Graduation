#pragma once

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
constexpr int MAX_PLAYER = 10;

#pragma pack(push,1)
struct playerInfo
{
	int id;
	Position m_position;
	bool connection = false;
};

struct recv_player_packet
{
	char key;
};

struct send_player_packet
{
	int x;
	int y;
};
#pragma pack (pop)