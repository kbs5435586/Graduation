#pragma once
// 서버 클라 같이 맞춰줘야 되는 모든건 프로토콜 헤더에
#define MY_NPC_START(p) 30 + (14 * p)
#define MY_NPC_END(p) 43 + (14 * p)

constexpr unsigned int MAX_PACKET_SIZE = 255;
constexpr unsigned int MAX_BUF_SIZE = 1024;

constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 80;
constexpr int MAX_CHAT_LEN = 50;

constexpr int MAX_USER = 29;
constexpr int NPC_ID_START = 30;
constexpr int MAX_NPC = 420;

constexpr int WORLD_HORIZONTAL = 10; // 월드 가로 x
constexpr int WORLD_HEIGHT = 10; // 월드 높이 y
constexpr int WORLD_VERTICAL = 10; // 월드 세로 z

constexpr int SERVER_PORT =	9000;

constexpr char CS_PACKET_LOGIN = 1;
constexpr char CS_PACKET_MOVE = 2;
constexpr char CS_PACKET_ADD_NPC = 3;
constexpr char CS_PACKET_NPC_ACT = 4;

constexpr char SC_PACKET_LOGIN_OK = 1;
constexpr char SC_PACKET_MOVE = 2;
constexpr char SC_PACKET_ENTER = 3;
constexpr char SC_PACKET_LEAVE = 4;
constexpr char SC_PACKET_CHAT = 5;
constexpr char SC_PACKET_ADD_NPC_OK = 6;

#pragma pack(push ,1)

struct sc_packet_login_ok 
{
	char size;
	char type;
	int id;
	float x, y, z;
	short hp;
	short level;
	int	exp;
};

struct sc_packet_move 
{
	char size;
	char type;
	int id;
	float x, y, z;
	unsigned move_time; // unsigned int랑 같음
};

constexpr unsigned char O_HUMAN = 0;
constexpr unsigned char O_ELF = 1;
constexpr unsigned char O_ORK = 2;

struct sc_packet_enter
{
	char size;
	char type;
	int id;
	char name[MAX_ID_LEN];
	char o_type;
	float x, y, z;
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
	char message[MAX_STR_LEN];
};

struct cs_packet_login
{
	char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

constexpr unsigned char GO_UP = 0;
constexpr unsigned char GO_DOWN = 1;
constexpr unsigned char GO_LEFT = 2;
constexpr unsigned char GO_RIGHT = 3;
constexpr unsigned char GO_FORWARD = 4;
constexpr unsigned char GO_BACK = 5;

struct cs_packet_move 
{
	char	size;
	char	type;
	char	direction;
	unsigned move_time;
};

struct cs_packet_add_npc
{
	char	size;
	char	type;
	int		id;
};

struct cs_packet_npc_act
{
	char	size;
	char	type;
	int		id;
	char	act;
};

constexpr unsigned char DO_ATTACK = 1;
constexpr unsigned char DO_DEFENCE = 2;
constexpr unsigned char DO_HOLD = 3;
constexpr unsigned char DO_FOLLOW = 4;
constexpr unsigned char DO_RANDMOVE = 5;

#pragma pack (pop)