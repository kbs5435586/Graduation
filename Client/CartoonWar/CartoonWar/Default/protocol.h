#pragma once
// 서버 클라 같이 맞춰줘야 되는 모든건 프로토콜 헤더에
#define MY_NPC_START(p) 30 + (14 * p)
#define MY_NPC_END(p) 43 + (14 * p)
enum Players {
	ENUM_PLAYER1, ENUM_PLAYER2, ENUM_PLAYER3, ENUM_PLAYER4, ENUM_PLAYER5, ENUM_PLAYER6, ENUM_PLAYER7, ENUM_PLAYER8,
	ENUM_PLAYER9, ENUM_PLAYER10, ENUM_PLAYER11, ENUM_PLAYER12, ENUM_PLAYER13, ENUM_PLAYER14, ENUM_PLAYER15, ENUM_PLAYER16,
	ENUM_PLAYER17, ENUM_PLAYER18, ENUM_PLAYER19, ENUM_PLAYER20, ENUM_PLAYER21, ENUM_PLAYER22, ENUM_PLAYER23, ENUM_PLAYER24,
	ENUM_PLAYER25, ENUM_PLAYER26, ENUM_PLAYER27, ENUM_PLAYER28, ENUM_PLAYER29, ENUM_PLAYER30, ENUM_PLAYER_END
};

constexpr unsigned int MAX_PACKET_SIZE = 255;
constexpr unsigned int MAX_BUF_SIZE = 1024;

const string SERVER_IP = "127.0.0.1"; // 123.215.109.236 // 127.0.0.1
constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 80;
constexpr int MAX_CHAT_LEN = 50;

constexpr int MAX_USER = 29;
constexpr int NPC_START = 30;
constexpr int MAX_NPC = 449;

constexpr int FLAG_START = 450;
constexpr int MAX_FLAG = 454;

constexpr int OBJECT_START = 450;
constexpr int MAX_OBJECT = 454;

constexpr int TEST_PLAYERS = 2;
constexpr int TEST_NPCS = 28;

constexpr int WORLD_HORIZONTAL = 500; // 월드 가로 x
constexpr int WORLD_HEIGHT = 400; // 월드 높이 y
constexpr int WORLD_VERTICAL = 500; // 월드 세로 z

constexpr int SERVER_PORT = 9000;

constexpr char CS_PACKET_LOGIN = 1;
constexpr char CS_PACKET_MOVE = 2;
constexpr char CS_PACKET_ROTATE = 3;
constexpr char CS_PACKET_ADD_NPC = 4;
constexpr char CS_PACKET_NPC_ACT = 5;
constexpr char CS_PACKET_CHANGE_FORMATION = 6;
constexpr char CS_PACKET_ATTACK = 7;
constexpr char CS_PACKET_ANIMATION = 8;
constexpr char CS_PACKET_POSITION = 9;

constexpr char SC_PACKET_LOGIN_OK = 1;
constexpr char SC_PACKET_MOVE = 2;
constexpr char SC_PACKET_ROTATE = 3;
constexpr char SC_PACKET_ENTER = 4;
constexpr char SC_PACKET_LEAVE = 5;
constexpr char SC_PACKET_CHAT = 6;
constexpr char SC_PACKET_ADD_NPC_OK = 7;
constexpr char SC_PACKET_ANIMATION = 8;
constexpr char SC_PACKET_ATTACKED = 9;
constexpr char SC_PACKET_DEAD = 10;
constexpr char SC_PACKET_ATTACK = 11;
constexpr char SC_PACKET_FLAG_INFO = 12;
constexpr char SC_PACKET_FLAG_BOOL = 13;
constexpr char SC_PACKET_TIME = 14;

#pragma pack(push ,1)

struct sc_packet_login_ok
{
	char size;
	char type;
	int id;
	float p_x, p_y, p_z;
	float r_x, r_y, r_z;
	float u_x, u_y, u_z;
	float l_x, l_y, l_z;
	short hp;
	short level;
	int	exp;
};

struct sc_packet_flag_info
{
	char size;
	char type;
	int id;
	bool isRed;
	bool isBlue;
	float p_x, p_y, p_z;
};

struct sc_packet_flag_bool
{
	char size;
	char type;
	int id;
	bool isRed;
	bool isBlue;
};

struct sc_packet_move
{
	char size;
	char type;
	int id;
	float x, y, z;
	//unsigned move_time; // 스트레스 테스트
};

constexpr unsigned char GO_UP = 0;
constexpr unsigned char GO_DOWN = 1;
constexpr unsigned char GO_LEFT = 2;
constexpr unsigned char GO_RIGHT = 3;
constexpr unsigned char GO_BACK = 4;
constexpr unsigned char GO_FORWARD = 5;
constexpr unsigned char GO_FAST_FORWARD = 6;
constexpr unsigned char TURN_LEFT = 7;
constexpr unsigned char TURN_RIGHT = 8;
constexpr unsigned char GO_COLLIDE = 9;

struct sc_packet_time
{
	char size;
	char type;
	float time;
};

struct sc_packet_rotate
{
	char size;
	char type;
	int id;
	float r_x, r_y, r_z;
	float u_x, u_y, u_z;
	float l_x, l_y, l_z;
};

constexpr unsigned char O_HUMAN = 0;
constexpr unsigned char O_ELF = 1;
constexpr unsigned char O_ORK = 2;

struct sc_packet_enter
{
	char size;
	char type;
	int id;
	short hp;
	char name[MAX_ID_LEN];
	char o_type;
	float p_x, p_y, p_z;
	float r_x, r_y, r_z;
	float u_x, u_y, u_z;
	float l_x, l_y, l_z;
};

struct sc_packet_leave
{
	char size;
	char type;
	int id;
};

struct sc_packet_attacked
{
	char size;
	char type;
	int id;
	short hp;
};

struct sc_packet_dead
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

struct sc_packet_npc_add_ok
{
	char	size;
	char	type;
	int		id;
	char	act;
};

struct sc_packet_animation
{
	char			size;
	char			type;
	unsigned char	anim;
	int				id;
};

struct cs_packet_animation
{
	char			size;
	char			type;
	unsigned char	anim;
};

constexpr unsigned char A_IDLE = 0;
constexpr unsigned char A_WALK = 1;
constexpr unsigned char A_CIDLE = 2;
constexpr unsigned char A_CWALK = 3;
constexpr unsigned char A_ATTACK = 4;
constexpr unsigned char A_HIT = 5;
constexpr unsigned char A_RUN = 6;
constexpr unsigned char A_CRUN = 7;
constexpr unsigned char A_DEAD = 8;

struct cs_packet_login
{
	char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

struct cs_packet_move
{
	char	size;
	char	type;
	char	direction;
	//unsigned move_time; // 스트레스 테스트
};

struct cs_packet_attack
{
	char	size;
	char	type;
};

struct cs_packet_position
{
	char	size;
	char	type;
	float	x;
	float	y;
	float	z;
};

struct cs_packet_rotate
{
	char	size;
	char	type;
	char	dir;
};

struct cs_packet_add_npc
{
	char	size;
	char	type;
};

struct cs_packet_npc_act
{
	char	size;
	char	type;
	int		id;
	char	act;
};

struct cs_packet_change_formation
{
	char	size;
	char	type;
};

constexpr unsigned char DO_ATTACK = 1;
constexpr unsigned char DO_DEFENCE = 2;
constexpr unsigned char DO_HOLD = 3;
constexpr unsigned char DO_FOLLOW = 4;
constexpr unsigned char DO_RANDMOVE = 5;

#pragma pack (pop)