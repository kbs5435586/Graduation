#pragma once

constexpr int MAX_ID_LEN = 10;

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
constexpr char SC_PACKET_LOGIN_OK = 0;
constexpr char SC_PACKET_MOVE = 1;
constexpr char SC_PACKET_ENTER = 2;
constexpr char SC_PACKET_LEAVE = 3;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;

constexpr char MV_UP = 0;
constexpr char MV_DOWN = 1;
constexpr char MV_LEFT = 2;
constexpr char MV_RIGHT = 3;

struct StoC_packet_move {
    char size;
    char type;
    int id;
    short x, y;
};

struct StoC_packet_login_ok
{
    char size;
    char type;
    int  id;
    short x, y;
    short hp;
    short level;
    int   exp;
};

struct CtoS_packet_login
{
    char  size;
    char  type;
    char  name[MAX_ID_LEN];
};

struct CtoS_packet_move
{
    char  size;
    char  type;
    char  direction;
};
#pragma pack (pop)


