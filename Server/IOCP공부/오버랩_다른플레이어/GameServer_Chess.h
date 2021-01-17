#pragma once
#include "resource.h"
#include "protocol.h"

struct SOCKETINFO
{
    SOCKET sock;
    char buf[MAX_BUF_SIZE + 1];
    int recvbytes;
    int sendbytes;
    BOOL recvdelayed;
    SOCKETINFO* next;
};

struct ClientInfo // 클라이언트 정보
{
    bool showCharacter;
	short m_x, m_y; // 나중에 맵이 256 범위 벗어날 수 있기 때문에 char로는 제한이 있음
	char m_name[MAX_ID_LEN + 1]; // +1은 아이디가 50 꽉차서 오면 안되긴 하지만 혹시라도 꽉 차서 왔을때 대비
};

void SocketEventMessage(HWND hWnd, LPARAM lParam);
BOOL InitServer(HWND hWnd);
void EndSocketConnect(SOCKET& socket);
void process_data(char* net_buf, size_t io_byte);
void ProcessPacket(char* ptr);
void send_move_packet(unsigned char dir);
void send_packet(void* packet);