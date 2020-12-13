#pragma once

constexpr int MAX_ID_LEN = 10;
constexpr int MAX_BUFFER = 4096;

typedef struct Position
{
	int x = 0;
	int y = 0;
};

typedef struct Client_Info
{
	char key;
	Position m_position;
	char  name[MAX_ID_LEN];
};

struct OVER_EX
{
	WSAOVERLAPPED wsa_over;
	WSABUF   wsa_buf; // 이 버퍼가 IOCP 버퍼를 가리키게 하면 된다. 얘도 오버랩드 I/O 하는동안 살아있어야 한다.  
	unsigned char iocp_buf[MAX_BUFFER]; // IOCP send, recv
	char   op_mode; // 어떤 용도로 사용됐는지 기록용 (send, recv, accept 구분)
};

#pragma pack (push, 1)

constexpr char SC_PACKET_LOGIN_OK = 0;
constexpr char SC_PACKET_MOVE = 1;
constexpr char SC_PACKET_ENTER = 2;
constexpr char SC_PACKET_LEAVE = 3;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;

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

struct StoC_packet_move 
{
	char size;
	char type;
	int id;
	short x, y;
	int move_time;
};

struct StoC_packet_enter 
{
	char size;
	char type;
	int  id;
	char name[MAX_ID_LEN];
	char over_type;
	short x, y;
};

struct StoC_packet_leave 
{
	char size;
	char type;
	int  id;
};

struct CtoS_packet_login 
{
	char  size;
	char  type;
	char  name[MAX_ID_LEN];
};

constexpr char MV_UP = 0;
constexpr char MV_DOWN = 1;
constexpr char MV_LEFT = 2;
constexpr char MV_RIGHT = 3;

struct CtoS_packet_move 
{
	char  size;
	char  type;
	char  direction;
	int	  move_time;
};

#pragma pack (pop)


