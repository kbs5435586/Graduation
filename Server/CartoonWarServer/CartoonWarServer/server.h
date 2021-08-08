#pragma once
#include "pch.h"

class Server
{
public:
	Server();
	~Server();

private:
	unordered_map <int, SESSION> g_clients;
	priority_queue<event_type> timer_queue;
	mutex timer_lock;
	Flag flags[5];

	SOCKET listenSocket;
	HANDLE g_iocp;

	int LISTEN_KEY = 999;

	float NEAR_APPROACH = 0.7f;
	int VIEW_RADIUS = 200; // ������ ������ �þ� ����
	float ATTACK_RADIUS = 7.f; // ������ ������ �þ� ����
	float DETECT_RADIUS = 15.f; // ������ ������ �þ� ����
	int BOID_RADIUS = 6;  // �÷��̾� ���� ���� ����
	float FLAG_RADIUS = 30.f;  // �÷��̾� ���� ���� ����

	float MOVE_TIME_ELAPSE = 0.0310692995f;
	float ROTATE_TIME_ELAPSE = 0.0286692995f;
	float MOVE_SPEED_NPC = 50.f;
	int FRAME_TIME = 16; // 1/4�ʿ� 1������, 60�������� 1/60�ʿ� 1�� ����, �뷫 16ms,17ms�ϸ� �ɵ�
	_vec3 SCALE = { 0.1f,0.1f,0.1f };
	int ATTACK_DAMAGE = 25;
	bool isGameStart;
	short StartGame_PlayerCount = 1;
	float play_time = 300;

public:
	void mainServer(); // ���� ����
	void error_display(const char* msg, int err_no);
	void start_game();

	void recv_packet_construct(int user_id, int io_byte); // ������ ��Ŷ�� Ŀ�� ©���� �� ��� ������ �Լ�
	void process_packet(int user_id, char* buf); // ��Ŷ ó�� ��ƾ
	void worker_thread();

	void send_packet(int user_id, void* packet); // ���� ��Ŷ�� ���ۿ� ������ ����
	void send_login_ok_packet(int user_id); // Ŭ��� ���� accept Ȯ�� �� Ŭ�� �ʱ�ȭ ��Ŷ ����
	void send_flag_info_packet(int object_id, int user_id); // ��� ��� ��ġ�� ����
	void send_flag_bool_packet(int object_id, int user_id); // ��� ��� ��ġ�� ����
	void send_fix_packet(int user_id, int other_id);
	void send_time_packet(); // ��� ��� ��ġ�� ����
	void send_condition_packet(int user_id, int other_id, unsigned char type); // ����� ��ġ�� ����
	void send_animation_packet(int user_id, int idler, unsigned char anim); // ����� ��ġ�� ����
	void send_enter_packet(int user_id, int other_id);
	void send_attacked_packet(int user_id, int other_id);
	void send_dead_packet(int user_id, int other_id);
	void send_leave_packet(int user_id, int other_id);
	void send_chat_packet(int lisn_id, int chat_id, char mess[]);
	void send_npc_size_packet(int user_id);
	void send_class_change_packet(int user_id, int other_id);

	void dead_reckoning(int player_id, ENUM_FUNCTION func_id); // �÷��̾� ���巹Ŀ��
	void do_animation(int user_id, unsigned char anim);
	void do_move(int user_id, char con); // Ŭ�󿡼� Ű �Է� �ް� ��ü �����̰� �Ҷ�
	void do_rotate(int user_id, char con);
	void set_formation(int user_id);
	void set_starting_pos(int user_id);
	void enter_game(int user_id, char name[]); // �ٸ� Ŭ��� ���� �˸�
	void initialize_clients(); // �÷��̾� �⺻ �ʱ�ȭ
	void initialize_objects(); // ��ü �ʱ�ȭ
	void disconnect(int user_id);

	void initialize_NPC(int player_id);
	void do_random_move(int npc_id);
	void do_follow(int npc_id);
	void do_attack(int npc_id);
	void do_dead(int id);
	void do_push(int pusher, int target);
	void do_change_formation(int player_id);
	void do_change_npc_act(int player_id, unsigned char act);
	void do_npc_rotate(int user_id, char con);
	void activate_npc(int npc_id, ENUM_FUNCTION op_type);
	void event_player_move(int player_id, int npc_id);
	void finite_state_machine(int npc_id, ENUM_FUNCTION func_id);
	_vec3 move_to_spot(int id, _vec3* goto_pos);
	float dist_between(int user_id, int other_id);
	float dist_between_finalPos(int user_id, int i);
	void update_speed(int user_id);
	void update_npc_troop(int npc_id);

	void add_timer(int obj_id, ENUM_FUNCTION op_type, int duration);
	void do_timer();

	bool is_near(int a, int b);
	bool is_attackable(int a, int b);
	bool is_player(int id);
	bool is_object(int id);
	void is_flag_near(int flag);
	bool is_attack_detect(int a, int b);
	bool check_basic_collision(int a, int b);
	bool check_obb_collision(int a, int b);

	//int API_SendMessage(lua_State* L);
	//int API_get_x(lua_State* L);
	//int API_get_y(lua_State* L);
public:
	void get_player_pos();
	void calculate_next_pos();
};

