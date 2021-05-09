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

	SOCKET listenSocket;
	HANDLE g_iocp;
	int LISTEN_KEY = 999;
	float NEAR_APPROACH = 0.3f;
	int VIEW_RADIUS = 200; // ������ ������ �þ� ����
	int BOID_RADIUS = 6;  // �÷��̾� ���� ���� ����

	float MOVE_SPEED_NPC = 0.4f;
	float MOVE_SPEED_PLAYER = 0.2f;
	float ROTATE_SPEED = 0.1f;
	int FRAME_TIME = 17; // 1/4�ʿ� 1������, 60�������� 1/60�ʿ� 1�� ����, �뷫 16ms,17ms�ϸ� �ɵ�
	float FORMATION_SPACE = 50.f;
	float SCALE_X = 0.05f, SCALE_Y = 0.05f, SCALE_Z = 0.05f;

public:
	void mainServer(); // ���� ����
	void error_display(const char* msg, int err_no);

	void recv_packet_construct(int user_id, int io_byte); // ������ ��Ŷ�� Ŀ�� ©���� �� ��� ������ �Լ�
	void process_packet(int user_id, char* buf); // ��Ŷ ó�� ��ƾ
	void worker_thread();

	void send_packet(int user_id, void* packet); // ���� ��Ŷ�� ���ۿ� ������ ����
	void send_login_ok_packet(int user_id); // Ŭ��� ���� accept Ȯ�� �� Ŭ�� �ʱ�ȭ ��Ŷ ����
	void send_move_packet(int user_id, int mover); // ����� ��ġ�� ����
	void send_rotate_packet(int user_id, int mover); // ����� ��ġ�� ����
	void send_enter_packet(int user_id, int other_id);
	void send_leave_packet(int user_id, int other_id);
	void send_chat_packet(int lisn_id, int chat_id, char mess[]);
	void send_npc_add_ok_packet(int user_id, int other_id); // Ŭ��� ���� accept Ȯ�� �� Ŭ�� �ʱ�ȭ ��Ŷ ����
	
	void do_move(int user_id, char direction); // Ŭ�󿡼� Ű �Է� �ް� ��ü �����̰� �Ҷ�
	void do_rotate(int user_id, char dir);
	void set_formation(int user_id);
	void enter_game(int user_id, char name[]); // �ٸ� Ŭ��� ���� �˸�
	void initialize_clients(); // ��ü ���� ������ �ʱ�ȭ
	void disconnect(int user_id);

	void initialize_NPC(int player_id);
	void do_random_move(int npc_id);
	void do_follow(int npc_id);
	void do_change_formation(int player_id);
	void do_npc_rotate(int user_id, char dir);
	void activate_npc(int npc_id, ENUM_FUNCTION op_type);
	void event_player_move(int player_id, int npc_id);
	void finite_state_machine(int npc_id, ENUM_FUNCTION func_id);
	_vec3 move_to_spot(int id, _vec3* goto_pos);

	void add_timer(int obj_id, ENUM_FUNCTION op_type, int duration);
	void do_timer();

	bool is_near(int a, int b);
	bool is_player(int id);
	bool check_collision(int a, int b);

	//int API_SendMessage(lua_State* L);
	//int API_get_x(lua_State* L);
	//int API_get_y(lua_State* L);
public:
	void get_player_pos();
	void calculate_next_pos();
};

