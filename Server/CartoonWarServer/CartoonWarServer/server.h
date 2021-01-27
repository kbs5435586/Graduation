#pragma once
#include "pch.h"

class Server
{
public:
	Server();
	~Server();

private:
	ClientInfo g_clients[MAX_USER];
	int current_User_ID = 0;
	HANDLE g_iocp;
	SOCKET listenSocket; // �������� �ѹ� �������� �ȹٲ�� �� (�����ͷ��̽�X, �����ص� ����)

	int LISTEN_KEY = 999;

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

	
	void do_move(int user_id, char direction); // Ŭ�󿡼� Ű �Է� �ް� ��ü �����̰� �Ҷ�
	void enter_game(int user_id); // �ٸ� Ŭ��� ���� �˸�

	void initalize_clients(); // ��ü ���� ������ �ʱ�ȭ
	void disconnect(int user_id);
};

