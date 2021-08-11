#pragma once
enum ENUM_FUNCTION
{
	FUNC_RECV, FUNC_SEND, FUNC_ACCEPT, FUNC_PLAYER_MOVE_FOR_NPC,
	FUNC_NPC_START, FUNC_NPC_ATTACK, FUNC_NPC_HOLD, FUNC_NPC_FOLLOW, 
	FUNC_CHECK_FLAG, FUNC_CHECK_TIME, FUNC_BATTLE, 
	FUNC_PLAYER_IDLE, FUNC_PLAYER_STRAIGHT, FUNC_PLAYER_BACK, FUNC_PLAYER_LEFT,
	FUNC_PLAYER_RIGHT, FUNC_PLAYER_RUN, FUNC_END
};
enum ENUM_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP, ST_DEAD, ST_END };
enum ENUM_MOVE { MV_UP, MV_DOWN, MV_LEFT, MV_RIGHT, MV_FORWARD, MV_BACK, MV_END };
enum ENUM_TEAM { TEAM_RED, TEAM_BLUE, TEAM_END };

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

struct Collision
{
	_vec3 col_range;
	float radius;
};

struct SESSION // Ŭ���̾�Ʈ ����
{
	// ����
	SOCKET m_socket;
	OverEx m_recv_over;
	int m_prev_size; // �߸� ������ ��� ������ �����ص� ���� ũ��
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv ������ �����ص� ����
	atomic <ENUM_STATUS> m_status;
	unsigned m_move_time; // ��Ʈ���� �׽�Ʈ

	// ����
	mutex m_cLock;
	int m_id;
	short m_hp;
	short m_team;
	unsigned char m_troop;
	unsigned char m_LastAnim;
	unsigned char m_anim;
	unsigned int m_class;

	ENUM_FUNCTION m_last_order;
	ENUM_FUNCTION m_curr_move;
	ENUM_FUNCTION m_curr_rotate;
	ENUM_FUNCTION m_last_move;
	ENUM_FUNCTION m_last_rotate;

	float m_move_speed;
	float m_rotate_speed;
	float m_total_angle;

	Collision m_col;
	unordered_set <int> m_view_list;

	char m_LastMcondition;
	char m_LastRcondition;
	char m_Mcondition;
	char m_Rcondition;
	CTransform m_transform;

	bool m_isFighting;
	
	// �÷��̾�
	vector <FormationInfo> m_boid;
	char m_formation;
	char m_name[MAX_ID_LEN + 1];
	char m_message[MAX_CHAT_LEN];

	// NPC
	short m_owner_id;
	bool m_isOut;
	bool m_isFormSet;
	unsigned short m_boid_num;
	_vec3 m_target_look;
	int m_attack_target;
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

struct Flag
{
	bool isRed;
	bool isBlue;
	_vec3 pos;
};

