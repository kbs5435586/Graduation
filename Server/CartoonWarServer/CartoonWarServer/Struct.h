#pragma once
#define PURE_EVENT -1

enum ENUM_FUNCTION
{
	FUNC_RECV, FUNC_SEND, FUNC_ACCEPT, FUNC_NPC_START, FUNC_NPC_ATTACK,
	FUNC_NPC_HOLD, FUNC_NPC_FOLLOW, FUNC_CHECK_FLAG, FUNC_CHECK_TIME,
	FUNC_DOT_DAMAGE, FUNC_CHECK_GOLD, FUNC_CHECK_COLLISION, FUNC_ARROW,
	FUNC_REVIVE, FUNC_NPC_LEAVE, FUNC_END
};
enum ENUM_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP, ST_DEAD, ST_END };
enum ENUM_TYPE { TP_PLAYER, TP_NPC, TP_ARROW, TP_DEFFEND, TP_NATURE, TP_SKILL, TP_ANIMAL, TP_END };
enum ENUM_TEAM { TEAM_RED, TEAM_BLUE, TEAM_END };

enum class COLLIDER_TYPE
{
	COLLIDER_AABB,
	COLLIDER_OBB,
	COLLIDER_SPHERE,
	COLLIDER_END
};

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

struct Teleport
{
	_vec3 telOne = {};
	_vec3 telTwo = {};
	int count = 0;
};

typedef struct tagOBB
{
	Vector3			vPoint[8];
	Vector3			vCenter;
	Vector3			vAlignAxis[3];
	Vector3			vProjAxis[3];
}OBB;

struct Collider
{
	_vec3	aabb_size;
	_vec3	obb_size;
	_vec3	m_ABvMin;
	_vec3	m_ABvMax;
	_vec3	m_OBvMin;
	_vec3	m_OBvMax;
	OBB		m_pOBB;
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
	int m_count;
	short m_hp;
	short m_team;
	unsigned char m_troop;
	unsigned char m_LastAnim;
	unsigned char m_anim;
	unsigned int m_class;

	ENUM_FUNCTION m_last_order;
	ENUM_TYPE m_type;

	int m_attack_target;
	float m_move_speed;
	float m_rotate_speed;
	float m_total_angle;
	float m_fBazierCnt;

	Collider m_col;
	_matrix m_matAttackedTarget;
	unordered_set <int> m_view_list;
	CTransform m_transform;

	bool m_isAttack; // Ʈ��� ���� �Ѱ�
	bool m_isOBB; // Ʈ��� ���� ���Ѱ�
	bool m_isBazier;

	_vec3	m_vStartPos = {};
	_vec3	m_vStartPoint = {};
	_vec3	m_vEndPoint = {};
	_vec3	m_vMidPoint = {};

	// �÷��̾�
	vector <FormationInfo> m_boid;
	char m_formation;
	char m_name[MAX_ID_LEN + 1];
	char m_message[MAX_CHAT_LEN];
	Teleport m_tel;
	int m_gold;

	// NPC
	short m_owner_id;
	bool m_isOut;
	bool m_isFormSet;
	unsigned short m_boid_num;
	_vec3 m_target_look;
	high_resolution_clock::time_point m_attacktime;

	// ��Ÿ
	high_resolution_clock::time_point m_lifetime;
};
