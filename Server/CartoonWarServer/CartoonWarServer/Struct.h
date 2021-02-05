#pragma once
enum ENUM_FUNCTION { FUNC_RECV, FUNC_SEND, FUNC_ACCEPT };
enum ENUM_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE };
// ���߿� ���� �߰� ����, Ŭ�� ������ ���������� Ŭ�� ����ü�� ���Ƽ� ��ó�� �ؾ��Ҷ��� ���� INACTIVE ��

struct OverEx // Ȯ�� ������ ����ü
{
	WSAOVERLAPPED over; // ������ ����ü
	ENUM_FUNCTION function; // send, recv, accept ������
	char io_buf[MAX_BUF_SIZE]; // IOCP send-recv ����
	union
	{
		WSABUF wsabuf;
		SOCKET c_socket;
	};
};

/*
OverEx�� �׳� SOCKET �ڷ��� ������ �߰��ص� ������µ�
OverEx ��ü�� ��û ���� ����ϴ� �ڷᱸ���̹Ƿ� ������� �߻� ����
union���� WSABUF wsabuf; SOCKET c_socket; �����ϸ� ���� �ʿ��Ѱ� �ϳ���
������ �� ����
*/

struct ClientInfo // Ŭ���̾�Ʈ ����
{
	mutex m_cLock;
	SOCKET m_socket;
	int m_id;
	OverEx m_recv_over;
	int m_prev_size; // �߸� ������ ��� ������ �����ص� ���� ũ��
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv ������ �����ص� ����
	ENUM_STATUS m_status;

	short m_x, m_y; // ���߿� ���� 256 ���� ��� �� �ֱ� ������ char�δ� ������ ����
	char m_name[MAX_ID_LEN + 1]; // +1�� ���̵� 50 ������ ���� �ȵǱ� ������ Ȥ�ö� �� ���� ������ ���
	// m_isConnected�� true�϶� m_name�� �ǹ�����, true�ε� m_name�� ���� ���� ��찡 �������
	unsigned m_move_time;
};