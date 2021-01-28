#pragma once
enum ENUM_FUNC { FUNC_RECV, FUNC_SEND, FUNC_ACCEPT };

struct OverEx // Ȯ�� ������ ����ü
{
	WSAOVERLAPPED over; // ������ ����ü
	ENUM_FUNC function; // send, recv, accept ������
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
	SOCKET m_socket;
	int m_id;
	OverEx m_recv_over;
	int m_prev_size; // �߸� ������ ��� ������ �����ص� ���� ũ��
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv ������ �����ص� ����
	bool m_isConnected; // �α��� ���� �Ǵܿ�

	short m_x, m_y; // ���߿� ���� 256 ���� ��� �� �ֱ� ������ char�δ� ������ ����
	char m_name[MAX_ID_LEN + 1]; // +1�� ���̵� 50 ������ ���� �ȵǱ� ������ Ȥ�ö� �� ���� ������ ���
};