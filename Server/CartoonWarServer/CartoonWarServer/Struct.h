#pragma once
enum ENUM_OP { OP_RECV, OP_SEND, OP_ACCEPT };

struct OverEx // 확장 오버랩 구조체
{
	WSAOVERLAPPED	over; // 오버랩 구조체
	WSABUF			wsabuf;
	ENUM_OP			op; // send, recv, accept 구별용
	char			io_buf[MAX_BUF_SIZE]; // IOCP send-recv 버퍼
};

struct ClientInfo // 클라이언트 정보
{
	SOCKET m_socket;
	int m_id;
	OverEx m_recv_over;
	int m_prev_size; // 잘린 파일의 경우 이전에 저장해둔 버퍼 크기
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv 성공시 저장해둘 버퍼

	short m_x, m_y; // 나중에 맵이 256 범위 벗어날 수 있기 때문에 char로는 제한이 있음
};