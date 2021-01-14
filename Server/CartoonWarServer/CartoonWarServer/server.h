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
	int LISTEN_KEY = 999;

public:
	void mainServer(); // ���� ����
	void process_packet(int user_id, char* buf); // ��Ŷ ó�� ��ƾ
	void send_login_ok_packet(int user_id); // Ŭ��� ���� accept Ȯ�� �� Ŭ�� �ʱ�ȭ ��Ŷ ����
	void send_packet(int user_id, void* packet); // ���� ��Ŷ�� ���ۿ� ������ ����
	void send_move_packet(int user_id); // ����� ��ġ�� ����
	void do_move(int user_id, char direction); // Ŭ�󿡼� Ű �Է� �ް� ��ü �����̰� �Ҷ�
};

