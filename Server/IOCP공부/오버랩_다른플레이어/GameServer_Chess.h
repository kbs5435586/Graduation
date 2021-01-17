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

struct ClientInfo // Ŭ���̾�Ʈ ����
{
    bool showCharacter;
	short m_x, m_y; // ���߿� ���� 256 ���� ��� �� �ֱ� ������ char�δ� ������ ����
	char m_name[MAX_ID_LEN + 1]; // +1�� ���̵� 50 ������ ���� �ȵǱ� ������ Ȥ�ö� �� ���� ������ ���
};

void SocketEventMessage(HWND hWnd, LPARAM lParam);
BOOL InitServer(HWND hWnd);
void EndSocketConnect(SOCKET& socket);
void process_data(char* net_buf, size_t io_byte);
void ProcessPacket(char* ptr);
void send_move_packet(unsigned char dir);
void send_packet(void* packet);