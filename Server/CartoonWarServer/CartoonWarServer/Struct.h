#pragma once
enum ENUM_FUNCTION
{
	FUNC_RECV, FUNC_SEND, FUNC_ACCEPT, FUNC_PLAYER_MOVE_FOR_NPC,
	FUNC_NPC_START, FUNC_NPC_RANDMOVE, FUNC_NPC_ATTACK, FUNC_NPC_DEFENCE,
	FUNC_NPC_HOLD, FUNC_NPC_FOLLOW, FUNC_END
};
enum ENUM_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP, ST_END };
enum ENUM_MOVE { MV_UP, MV_DOWN, MV_LEFT, MV_RIGHT, MV_FORWARD, MV_BACK, MV_END };
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
		int player_id; // Ÿ�̸ӷ� �Ѱܹ��� ��ü ���̵�
	};
};

struct Vec3
{
	float x;
	float y;
	float z;

	Vec3 operator+(Vec3& num)
	{
		return { x + num.x, y + num.y, z + num.z };
	}
	Vec3 operator-(Vec3& num)
	{
		return { x - num.x, y - num.y, z - num.z };
	}
	Vec3 operator*(Vec3& num)
	{
		return { x * num.x, y * num.y, z * num.z };
	}
	Vec3 operator*(float num)
	{
		return { x * num, y * num, z * num };
	}
	Vec3 operator/(Vec3& num)
	{
		return { x / num.x, y / num.y, z / num.z };
	}
	Vec3 operator/(float num)
	{
		return { x / num, y / num, z / num };
	}
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
	int m_owner_id;
	ENUM_FUNCTION m_last_order;
	OverEx m_recv_over;
	int m_prev_size; // �߸� ������ ��� ������ �����ص� ���� ũ��
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv ������ �����ص� ����
	atomic <ENUM_STATUS> m_status;

	Vec3 m_pos;
	//float m_x, m_y, m_z; // ���߿� ���� 256 ���� ��� �� �ֱ� ������ char�δ� ������ ����
	char m_name[MAX_ID_LEN + 1]; // +1�� ���̵� 50 ������ ���� �ȵǱ� ������ Ȥ�ö� �� ���� ������ ���
	// m_isConnected�� true�϶� m_name�� �ǹ�����, true�ε� m_name�� ���� ���� ��찡 �������
	char m_message[MAX_CHAT_LEN];
	unsigned m_move_time;
	high_resolution_clock::time_point m_last_move_time;


	unordered_set <int> m_view_list;
	// �׳� set�� iter �������� ������� �������� unordered_set�� ������� �ȳ���, �ٵ� �丮��Ʈ ��ü�� �������X
};

struct event_type
{
	int obj_id; // � ��ü�� �������� �ϴ°�
	int event_id; // � �̺�Ʈ�ΰ�
	high_resolution_clock::time_point wakeup_time; // ���� �� �̺�Ʈ�� ����Ǿ� �ϴ°�
	int target_id; // �߻��ϴ� �߰�����

	constexpr bool operator < (const event_type& left) const
	{
		return (wakeup_time > left.wakeup_time); // ������� �����ϴ� �뵵
	}
};