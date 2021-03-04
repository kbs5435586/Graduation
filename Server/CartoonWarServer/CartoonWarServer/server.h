#pragma once
#include "pch.h"

class Server
{
public:
	Server();
	~Server();

private:
	map <int, ClientInfo> g_clients;
	priority_queue<event_type> timer_queue;
	mutex timer_lock;
	ENUM_FUNCTION old_machine;

	SOCKET listenSocket;
	HANDLE g_iocp;
	int LISTEN_KEY = 999;
	int VIEW_RADIUS = 6;

public:
	void mainServer(); // ���� ����
	void error_display(const char* msg, int err_no);

	void recv_packet_construct(int user_id, int io_byte); // ������ ��Ŷ�� Ŀ�� ©���� �� ��� ������ �Լ�
	void process_packet(int user_id, char* buf); // ��Ŷ ó�� ��ƾ
	void worker_thread();

	void send_packet(int user_id, void* packet); // ���� ��Ŷ�� ���ۿ� ������ ����
	void send_login_ok_packet(int user_id); // Ŭ��� ���� accept Ȯ�� �� Ŭ�� �ʱ�ȭ ��Ŷ ����
	void send_move_packet(int user_id, int mover); // ����� ��ġ�� ����
	void send_enter_packet(int user_id, int other_id);
	void send_leave_packet(int user_id, int other_id);
	void send_chat_packet(int lisn_id, int chat_id, char mess[]);
	
	void do_move(int user_id, char direction); // Ŭ�󿡼� Ű �Է� �ް� ��ü �����̰� �Ҷ�
	void enter_game(int user_id, char name[]); // �ٸ� Ŭ��� ���� �˸�
	void initalize_clients(); // ��ü ���� ������ �ʱ�ȭ
	void disconnect(int user_id);

	void initalize_NPC(int player_id);
	void do_random_move(int npc_id);
	void do_follow(int npc_id);
	void activate_npc(int npc_id, ENUM_FUNCTION op_type);
	void event_player_move(int player_id, int npc_id);
	void finite_state_machine(int npc_id, ENUM_FUNCTION func_id);

	void add_timer(int obj_id, ENUM_FUNCTION op_type, int duration);
	void do_timer();

	bool is_near(int a, int b);
	bool is_player(int id);

	//int API_SendMessage(lua_State* L);
	//int API_get_x(lua_State* L);
	//int API_get_y(lua_State* L);
public:
	void init_flock();
	void get_player_pos();
	void calculate_next_pos();
};

