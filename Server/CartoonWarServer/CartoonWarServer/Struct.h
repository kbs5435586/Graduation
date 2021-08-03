#pragma once
enum ENUM_FUNCTION
{
	FUNC_RECV, FUNC_SEND, FUNC_ACCEPT, FUNC_PLAYER_MOVE_FOR_NPC,
	FUNC_NPC_START, FUNC_NPC_RANDMOVE, FUNC_NPC_ATTACK, FUNC_NPC_DEFENCE,
	FUNC_NPC_HOLD, FUNC_NPC_FOLLOW, FUNC_DEAD, FUNC_CHECK_FLAG, FUNC_CHECK_TIME, 
	FUNC_PLAYER_IDLE, FUNC_PLAYER_STRAIGHT, FUNC_PLAYER_BACK, FUNC_PLAYER_LEFT,
	FUNC_PLAYER_RIGHT, FUNC_PLAYER_RUN, FUNC_END
};
enum ENUM_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE, ST_SLEEP, ST_DEAD, ST_END };
enum ENUM_MOVE { MV_UP, MV_DOWN, MV_LEFT, MV_RIGHT, MV_FORWARD, MV_BACK, MV_END };
enum ENUM_TEAM { TEAM_RED, TEAM_BLUE, TEAM_END };

// 나중에 상태 추가 가능, 클라 접속이 끊어졌지만 클라 구조체가 남아서 뒷처리 해야할때가 있음 INACTIVE 등

struct OverEx // 확장 오버랩 구조체
{
	WSAOVERLAPPED over; // 오버랩 구조체
	ENUM_FUNCTION function; // send, recv, accept 구별용
	char io_buf[MAX_BUF_SIZE]; // IOCP send-recv 버퍼
	union
	{
		WSABUF wsabuf;
		SOCKET c_socket;
		int player_id; // 타이머로 넘겨받을 객체 아이디
	};
};

struct Collision
{
	_vec3 col_range;
	float radius;
};

struct SESSION // 클라이언트 정보
{
	mutex m_cLock;
	SOCKET m_socket;
	int m_id;
	short m_owner_id;
	short m_hp;
	short m_team;
	short m_select_cls;
	unsigned char m_LastAnim;
	unsigned char m_anim;
	ENUM_FUNCTION m_last_order;
	ENUM_FUNCTION m_curr_move;
	ENUM_FUNCTION m_curr_rotate;
	ENUM_FUNCTION m_last_move;
	ENUM_FUNCTION m_last_rotate;
	OverEx m_recv_over;
	int m_prev_size; // 잘린 파일의 경우 이전에 저장해둔 버퍼 크기
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv 성공시 저장해둘 버퍼
	atomic <ENUM_STATUS> m_status;
	unsigned m_move_time; // 스트레스 테스트

	float m_move_speed;
	float m_rotate_speed;
	float m_total_angle;
	bool m_isOut;
	bool m_isFormSet;
	
	Collision m_col;
	vector <FormationInfo> m_boid;
	unsigned short m_boid_num;
	char m_formation;
	unsigned int m_class;
	char m_LastMcondition;
	char m_LastRcondition;
	char m_Mcondition;
	char m_Rcondition;

	_vec3 m_target_pos;
	CTransform m_transform;
	//float m_x, m_y, m_z; // 나중에 맵이 256 범위 벗어날 수 있기 때문에 char로는 제한이 있음
	char m_name[MAX_ID_LEN + 1]; // +1은 아이디가 50 꽉차서 오면 안되긴 하지만 혹시라도 꽉 차서 왔을때 대비
	// m_isConnected가 true일때 m_name가 의미있음, true인데 m_name에 값이 없는 경우가 없어야함
	char m_message[MAX_CHAT_LEN];

	unordered_set <int> m_view_list;
	// 그냥 set은 iter 돌렸을때 순서대로 나오지만 unordered_set은 순서대로 안나옴, 근데 뷰리스트 자체가 순서상관X
};

struct event_type
{
	int obj_id; // 어떤 객체가 움직여야 하는가
	int event_id; // 어떤 이벤트인가
	high_resolution_clock::time_point wakeup_time; // 언제 이 이벤트가 실행되야 하는가
	int target_id; // 발생하는 추가정보

	constexpr bool operator < (const event_type& left) const
	{
		return (wakeup_time > left.wakeup_time); // 순서대로 저장하는 용도
	}
};

struct Flag
{
	bool isRed;
	bool isBlue;
	_vec3 pos;
};

