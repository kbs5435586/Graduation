#pragma once
#include "pch.h"

class Server
{
public:
	Server();
	~Server();

private:
	LARGE_INTEGER   m_frameTime;
	LARGE_INTEGER   m_lastTime;
	LARGE_INTEGER   m_fixTime;
	LARGE_INTEGER   m_CPUTick;

	unordered_map <int, SESSION> g_clients;
	priority_queue<event_type> timer_queue;
	mutex timer_lock;
	Flag flags[5];

	SOCKET listenSocket;
	HANDLE g_iocp;

	int LISTEN_KEY = 999;

	float NEAR_APPROACH = 0.5f;
	int VIEW_RADIUS = 500; // 데이터 보내줄 시야 범위
	float ATTACK_RADIUS = 4.f; // 데이터 보내줄 시야 범위
	float DETECT_RADIUS = 20.f; // 데이터 보내줄 시야 범위
	int BOID_RADIUS = 6;  // 플레이어 기준 군집 범위
	float FLAG_RADIUS = 10.f;  // 플레이어 기준 군집 범위
	float TIME_DELTA = 0.f;
	int ARROW_COUNT = 0;
	int REVIVE_COOLTIME = 3000;
	int NPC_LEAVE_TIME = 1500;
	float ARROW_ENDTIME = 3.f;
	float ARROW_DIST = 10.f;
	float ARROW_DAMAGE = 10.f;
	int FIRE_SKILL_TIME = 5;

	int FRAME_TIME = 33; // 1/4초에 1번전송, 60프레임은 1/60초에 1번 전송, 대략 16ms,17ms하면 될듯
	_vec3 SCALE = { 0.1f,0.1f,0.1f };
	int ATTACK_DAMAGE = 10;
	float FLAME_RANGE = 50.f;
	int DOT_DAMAGE = 5;
	int SET_HP = 100;
	bool isGameStart;
	int StartGame_PlayerCount = 1;
	float play_time = 0;

	float OBB_DIST = 5.f;
	float AABB_PN = 4.f;
	float AABB_NAT = 50.f;
	float AABB_DEF = 10.f;

public:
	void mainServer(); // 메인 서버
	void error_display(const char* msg, int err_no);
	const float time_delta();
	void ready_timer();

	void recv_packet_construct(int user_id, int io_byte); // 수신한 패킷이 커서 짤려서 온 경우 재조립 함수
	void process_packet(int user_id, char* buf); // 패킷 처리 루틴
	void worker_thread();

	void send_packet(int user_id, void* packet); // 보낼 패킷의 버퍼와 사이즈 설정
	void send_login_ok_packet(int user_id); // 클라로 부터 accept 확인 시 클라 초기화 패킷 설정
	void send_flag_info_packet(int object_id, int user_id); // 모든 깃발 위치값 전송
	void send_flag_bool_packet(int object_id, int user_id); // 모든 깃발 위치값 전송
	void send_time_packet(); // 모든 깃발 위치값 전송
	void send_move_packet(int user_id, int other_id); // 변경된 위치값 설정
	void send_animation_packet(int user_id, int idler, unsigned char anim); // 변경된 위치값 설정
	void send_enter_packet(int user_id, int other_id);
	void send_dead_packet(int user_id, int other_id);
	void send_leave_packet(int user_id, int other_id);
	void send_npc_size_packet(int user_id);
	void send_class_change_packet(int user_id, int other_id);
	void send_invisible_packet(int user_id, int other_id, bool invi);
	void send_fire_packet(int id, float mx, float mz);
	void send_teleport_packet(int id, float mx, float mz, unsigned char m);
	void send_time_delta(int user_id, float time);
	void send_gold_packet(int user_id);
	void send_hp_packet(int user_id, int other_id);
	void send_do_particle_packet(int user_id, int other_id);
	void send_arrow_packet(int arrow_id, int user_id, int other_id);
	void send_deffend_packet(int deffend_id, int user_id, int other_id);
	void send_revive_packet(int user_id, int other_id);
	void send_nature_scale_packet(int user_id, int other_id);

	void set_formation(int user_id);
	void set_starting_pos(int user_id);
	void enter_game(int user_id, char name[]); // 다른 클라들 입장 알림
	void disconnect(int user_id);

	void do_animation(int user_id, unsigned char anim);
	void do_move(int user_id, char con); // 클라에서 키 입력 받고 객체 움직이게 할때
	void do_follow(int npc_id);
	void do_attack(int npc_id);
	void do_dead(int id);
	void do_revive(int id);
	void do_change_formation(int player_id);
	void do_change_npc_act(int player_id, unsigned char act);
	void do_fire_skill_damage(int id);
	void do_aabb(int o_mv, int o_ht);

	void do_arrow(int arrow_id);
	void delete_arrow(int arrow_id);
	void do_arrow_collision(int arrow_id);

	void activate_npc(int npc_id, ENUM_FUNCTION op_type);
	void finite_state_machine(int npc_id, ENUM_FUNCTION func_id);
	_vec3 move_to_spot(int id, _vec3* goto_pos);
	float dist_between(int user_id, int other_id);
	float dist_between_finalPos(int user_id, int i);
	void update_speed_and_collider(int user_id);
	void update_npc_troop(int npc_id);

	void add_timer(int obj_id, ENUM_FUNCTION op_type, int duration);
	void do_event_timer();

	bool is_near(int a, int b);
	bool is_attackable(int a, int b);
	bool is_player(int id);
	bool is_object(int id);
	void is_flag_near(int flag);
	bool is_attack_detect(int a, int b);
	bool is_attack_view(int attack, int gethit);

	void Update_Collider(int id, _vec3 vSize, COLLIDER_TYPE eType); // aabb 및 obb min, max 값 업데이트 용도
	_matrix Compute_WorldTransform(int id);
	_matrix Remove_Rotation(_matrix matWorld);
	void check_aabb_collision(int a, int b);
	bool check_obb_collision(int a, int b);
	void Ready_Collider_AABB_BOX(int id, const _vec3 vSize);
	void Ready_Collider_OBB_BOX(int id, const _vec3 vSize);
	void Compute_AlignAxis(OBB* pOBB);
	void Compute_ProjAxis(OBB* pOBB);
	void Obb_Collision(int id);
	void Hit_Object(int id, _float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid);

	void initialize_objects(); // 객체 초기화
	void initialize_NPC(int player_id);
	void initialize_nature();
	int init_arrow(int shoot_id);

	void cal_change_class_gold(int id, short m_class);
};

