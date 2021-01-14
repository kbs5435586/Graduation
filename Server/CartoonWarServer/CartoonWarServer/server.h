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
	void mainServer(); // 메인 서버
	void process_packet(int user_id, char* buf); // 패킷 처리 루틴
	void send_login_ok_packet(int user_id); // 클라로 부터 accept 확인 시 클라 초기화 패킷 설정
	void send_packet(int user_id, void* packet); // 보낼 패킷의 버퍼와 사이즈 설정
	void send_move_packet(int user_id); // 변경된 위치값 설정
	void do_move(int user_id, char direction); // 클라에서 키 입력 받고 객체 움직이게 할때
};

