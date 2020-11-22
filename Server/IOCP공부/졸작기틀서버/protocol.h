#pragma once

typedef struct Position
{
	int x = 0;
	int y = 0;
};

typedef struct Client_Info
{
	char key;
	Position m_position;
};

#pragma pack(push,1)

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


