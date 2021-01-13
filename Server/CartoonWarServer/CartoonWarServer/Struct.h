#pragma once
enum ENUM_OP { OP_RECV, OP_SEND, OP_ACCEPT };

struct OverEx // Ȯ�� ������ ����ü
{
	WSAOVERLAPPED	over; // ������ ����ü
	WSABUF			wsabuf;
	ENUM_OP			op; // send, recv, accept ������
	char			io_buf[MAX_BUF_SIZE]; // IOCP send-recv ����
};

struct ClientInfo // Ŭ���̾�Ʈ ����
{
	SOCKET m_socket;
	int m_id;
	OverEx m_recv_over;
	int m_prev_size; // �߸� ������ ��� ������ �����ص� ���� ũ��
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv ������ �����ص� ����

	short m_x, m_y; // ���߿� ���� 256 ���� ��� �� �ֱ� ������ char�δ� ������ ����
};