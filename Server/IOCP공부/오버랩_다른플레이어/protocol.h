#pragma once

constexpr int MAX_PLAYER = 10;

typedef struct Position
{
	int x = 0;
	int y = 0;
};

typedef struct Key_Info
{
	char key;
};

#pragma pack(push,1)
struct Client_Info
{
	char key;
	int id;
	Position m_position;
};

struct send_player_packet
{
	char key;
};

struct recv_player_packet
{
	int x;
	int y;
};
#pragma pack (pop)