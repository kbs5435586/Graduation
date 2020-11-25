#pragma once

struct clientData
{
	int id;
	char name[MAX_ID_LEN];
	short x, y;

	bool in_use;
	SOCKET   m_sock;
	OVER_EX   m_recv_over;
	unsigned char* m_packet_start;
	unsigned char* m_recv_start;
};