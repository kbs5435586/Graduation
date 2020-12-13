#pragma once
#pragma once

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
constexpr int MAX_PLAYER = 10;
constexpr int  KEY_SERVER = 1000000;
constexpr int MAX_BUFFER = 4096;
constexpr int MAX_ID_LEN = 10;
constexpr int MIN_BUFF_SIZE = 1024;

constexpr int WORLD_WIDTH = 8;
constexpr int WORLD_HEIGHT = 8;

constexpr char OP_MODE_RECV = 0;
constexpr char OP_MODE_SEND = 1;
constexpr char OP_MODE_ACCEPT = 2;

struct OVER_EX
{
    WSAOVERLAPPED wsa_over;
    WSABUF   wsa_buf; // �� ���۰� IOCP ���۸� ����Ű�� �ϸ� �ȴ�. �굵 �������� I/O �ϴµ��� ����־�� �Ѵ�.  
    unsigned char iocp_buf[MAX_BUFFER]; // IOCP send, recv
    char   op_mode; // � �뵵�� ���ƴ��� ��Ͽ� (send, recv, accept ����)
};

struct clientData
{
    int id;
    char name[MAX_ID_LEN];
    short x, y;
    bool in_use;

    SOCKET   m_sock;
    OVER_EX   m_recv_over; // Ŭ���̾�Ʈ ��ü�� ������ �������� ����ü �ϳ��� �ݵ�� �ʿ���
    unsigned char* m_packet_start;
    unsigned char* m_recv_start;
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

struct StoC_packet_move
{
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