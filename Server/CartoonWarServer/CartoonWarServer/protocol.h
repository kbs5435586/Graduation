#pragma once
// 서버 클라 같이 맞춰줘야 되는 모든건 프로토콜 헤더에
#define MY_NPC_START_SERVER(p) 30 + (9 * p)
#define MY_NPC_END_SERVER(p) 38 + (9 * p)

#define MY_NPC_START_CLIENT(p) (9 * p)
#define MY_NPC_END_CLIENT(p) 8 + (9 * p)

enum PLAYERS {
	ENUM_PLAYER1, ENUM_PLAYER2, ENUM_PLAYER3, ENUM_PLAYER4, ENUM_PLAYER5, ENUM_PLAYER6, ENUM_PLAYER7, ENUM_PLAYER8,
	ENUM_PLAYER9, ENUM_PLAYER10, ENUM_PLAYER11, ENUM_PLAYER12, ENUM_PLAYER13, ENUM_PLAYER14, ENUM_PLAYER15, ENUM_PLAYER16,
	ENUM_PLAYER17, ENUM_PLAYER18, ENUM_PLAYER19, ENUM_PLAYER20, ENUM_PLAYER21, ENUM_PLAYER22, ENUM_PLAYER23, ENUM_PLAYER24,
	ENUM_PLAYER25, ENUM_PLAYER26, ENUM_PLAYER27, ENUM_PLAYER28, ENUM_PLAYER29, ENUM_PLAYER30, ENUM_PLAYER_END
};

struct FormationInfo
{
	int id;
	_vec3 final_pos;
	float angle;
	float radius;
};

constexpr unsigned int MAX_PACKET_SIZE = 255;
constexpr unsigned int MAX_BUF_SIZE = 512;

const string SERVER_IP = "127.0.0.1"; // 123.215.109.236 // 127.0.0.1
constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 80;
constexpr int MAX_CHAT_LEN = 50;

constexpr int MAX_USER = 29;
constexpr int NPC_START = 30;
constexpr int MAX_NPC = 449;
constexpr int MAX_OWN_NPC = 9;

constexpr int FLAG_START = 450;
constexpr int MAX_FLAG = 454;

constexpr int OBJECT_START = 450;
constexpr int MAX_OBJECT = 454;

constexpr int CL_TEST_PLAYERS = 4;
constexpr int CL_TEST_NPCS = 36;
constexpr int CL_NPC_TEAM1 = 9;
constexpr int CL_NPC_TEAM2 = 18;
constexpr int CL_NPC_TEAM3 = 27;
constexpr int CL_NPC_TEAM4 = 36;

constexpr int WORLD_HORIZONTAL = 500; // 월드 가로 x
constexpr int WORLD_HEIGHT = 400; // 월드 높이 y
constexpr int WORLD_VERTICAL = 500; // 월드 세로 z

constexpr int SERVER_PORT = 9000;

constexpr float BASIC_FORM_RAD = 10.f;
constexpr float PIE = 3.141592f;

constexpr char CS_PACKET_LOGIN = 1;
constexpr char CS_PACKET_CONDITION = 2;
constexpr char CS_PACKET_ADD_NPC = 3;
constexpr char CS_PACKET_NPC_ACT = 4;
constexpr char CS_PACKET_CHANGE_FORMATION = 5;
constexpr char CS_PACKET_ATTACK = 6;
constexpr char CS_PACKET_ANIMATION = 7;
constexpr char CS_PACKET_POSITION = 8;
constexpr char CS_PACKET_CLASS_CHANGE = 9;
constexpr char CS_PACKET_TROOP_CHANGE = 10;

constexpr char SC_PACKET_LOGIN_OK = 1;
constexpr char SC_PACKET_CONDITION = 2;
constexpr char SC_PACKET_ENTER = 3;
constexpr char SC_PACKET_LEAVE = 4;
constexpr char SC_PACKET_CHAT = 5;
constexpr char SC_PACKET_CLASS_CHANGE = 6;
constexpr char SC_PACKET_ANIMATION = 7;
constexpr char SC_PACKET_ATTACKED = 8;
constexpr char SC_PACKET_DEAD = 9;
constexpr char SC_PACKET_ATTACK = 10;
constexpr char SC_PACKET_FLAG_INFO = 11;
constexpr char SC_PACKET_FLAG_BOOL = 12;
constexpr char SC_PACKET_TIME = 13;
constexpr char SC_PACKET_FIX = 14;
constexpr char SC_PACKET_NPC_SIZE = 15;
constexpr char SC_PACKET_HIT = 16;

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
	short p_class;
};

constexpr unsigned short C_WORKER = 0;
constexpr unsigned short C_CAVALRY = 1;
constexpr unsigned short C_TWO = 2;
constexpr unsigned short C_INFANTRY = 3;
constexpr unsigned short C_FOUR = 4;
constexpr unsigned short C_SPEARMAN = 5;
constexpr unsigned short C_MAGE = 6;
constexpr unsigned short C_MMAGE = 7;
constexpr unsigned short C_ARCHER = 8;

constexpr unsigned char T_INFT = 0;
constexpr unsigned char T_HORSE = 1;
constexpr unsigned char T_MAGE = 2;
constexpr unsigned char T_BOW = 3;
constexpr unsigned char T_ALL = 4;

constexpr unsigned char F_SQUARE = 0;
constexpr unsigned char F_PIRAMID = 1;
constexpr unsigned char F_CIRCLE = 2;
constexpr unsigned char F_FLOCK = 3;

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

struct sc_packet_time
{
	char size;
	char type;
	float time;
};

struct sc_packet_hit
{
	char size;
	char type;
	int id;
	bool ishit;
};

constexpr unsigned char O_PLAYER = 0;
constexpr unsigned char O_NPC = 1;
constexpr unsigned char O_ENVIR = 2;

struct sc_packet_enter
{
	char size;
	char type;
	int id;
	short hp;
	char name[MAX_ID_LEN];
	char con_move;
	char con_rotate;
	float p_x, p_y, p_z;
	float r_x, r_y, r_z;
	float u_x, u_y, u_z;
	float l_x, l_y, l_z;
	short p_class;
};

struct sc_packet_change_class
{
	char size;
	char type;
	int id;
	short p_class;
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
	bool ishit;
};

struct sc_packet_condition
{
	char size;
	char type;
	char con_type;
	int id;
	char condition;
};

struct sc_packet_fix
{
	char size;
	char type;
	int id;
	float r_x, r_y, r_z;
	float u_x, u_y, u_z;
	float l_x, l_y, l_z;
	float p_x, p_z;
};

constexpr unsigned char CON_TYPE_MOVE = 0;
constexpr unsigned char CON_TYPE_ROTATE = 1;

constexpr unsigned char CON_IDLE = 0;
constexpr unsigned char CON_STRAIGHT = 1;
constexpr unsigned char CON_BACK = 2;
constexpr unsigned char CON_LEFT = 3;
constexpr unsigned char CON_RIGHT = 4;
constexpr unsigned char CON_RUN = 5;

struct sc_packet_dead
{
	char size;
	char type;
	int id;
	char anim;
	short hp;
};

struct sc_packet_chat
{
	char size;
	char type;
	int	 id;
	char message[MAX_STR_LEN];
};

struct sc_packet_npc_size
{
	char			size;
	char			type;
	unsigned short	npc_size;
};

struct sc_packet_animation
{
	char			size;
	char			type;
	unsigned char	anim;
	int				id;
};

struct sc_packet_class_change
{
	char size;
	char type;
	int id;
	short p_class;
};

struct cs_packet_class_change
{
	char size;
	char type;
	int id;
	short p_class;
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
constexpr unsigned char A_CAST = 9;

struct cs_packet_login
{
	char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

struct cs_packet_condition
{
	char	size;
	char	type;
	char	con_type;
	char	con;
	//unsigned move_time; // 스트레스 테스트
};

constexpr unsigned char GO_UP = 0;
constexpr unsigned char GO_DOWN = 1;
constexpr unsigned char GO_LEFT = 2;
constexpr unsigned char GO_RIGHT = 3;
constexpr unsigned char GO_BACK = 4;
constexpr unsigned char GO_FORWARD = 5;
constexpr unsigned char GO_FAST_FORWARD = 6;
constexpr unsigned char GO_COLLIDE = 9;

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

struct cs_packet_add_npc
{
	char	size;
	char	type;
};

struct cs_packet_change_troop
{
	char	size;
	char	type;
	char	troop;
};

struct cs_packet_npc_act
{
	char	size;
	char	type;
	char	act;
};

struct cs_packet_change_formation
{
	char	size;
	char	type;
};

constexpr unsigned char DO_FOLLOW = 0;
constexpr unsigned char DO_ATTACK = 1;
constexpr unsigned char DO_HOLD = 2;

#pragma pack (pop)