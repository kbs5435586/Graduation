#pragma once
// 서버 클라 같이 맞춰줘야 되는 모든건 프로토콜 헤더에

constexpr unsigned int MAX_PACKET_SIZE = 255;
constexpr unsigned int MAX_BUF_SIZE = 1024;

constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 255;
constexpr int MAX_USER = 10;

constexpr int WORLD_WIDTH =	8;
constexpr int WORLD_HEIGHT = 8;

constexpr int SERVER_PORT =	9000;

constexpr char C2S_LOGIN = 1;
constexpr char C2S_MOVE = 2;

constexpr char S2C_LOGIN_OK = 1;
constexpr char S2C_MOVE = 2;
constexpr char S2C_ENTER = 3;
constexpr char S2C_LEAVE = 4;

#pragma pack(push ,1)

struct sc_packet_login_ok 
{
	char size;
	char type;
	int id;
	short x, y;
	short hp;
	short level;
	int	exp;
};

struct sc_packet_move 
{
	char size;
	char type;
	int id;
	short x, y;
};

struct sc_packet_enter
{
	char size;
	char type;
	int id;
	char name[MAX_ID_LEN];
	char o_type;
	short x, y;
};

struct sc_packet_leave 
{
	char size;
	char type;
	int id;
};

struct sc_packet_chat
{
	char size;
	char type;
	int	 id;
	char chat[100];
};

struct cs_packet_login
{
	char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

constexpr unsigned char D_UP = 0;
constexpr unsigned char D_DOWN = 1;
constexpr unsigned char D_LEFT = 2;
constexpr unsigned char D_RIGHT = 3;

struct cs_packet_move 
{
	char	size;
	char	type;
	char	direction;
};

#pragma pack (pop)