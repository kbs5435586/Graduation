#pragma once

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
constexpr int MAX_PLAYER = 10;
constexpr int  KEY_SERVER = 1000000;
constexpr int MAX_BUFFER = 4096;
constexpr int MAX_ID_LEN = 10;

constexpr int WORLD_WIDTH = 8;
constexpr int WORLD_HEIGHT = 8;

constexpr char OP_MODE_RECV = 0;
constexpr char OP_MODE_SEND = 1;
constexpr char OP_MODE_ACCEPT = 2;

struct clientData {
    int id;
    char name[MAX_ID_LEN];
    short x, y;

    bool in_use;
    SOCKET   m_sock;
    OVER_EX   m_recv_over;
    unsigned char* m_packet_start;
    unsigned char* m_recv_start;
};

struct OVER_EX {
    WSAOVERLAPPED wsa_over;
    char   op_mode;
    WSABUF   wsa_buf;
    unsigned char iocp_buf[MAX_BUFFER];
};

#pragma pack(push,1)
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