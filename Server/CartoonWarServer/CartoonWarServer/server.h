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
	SOCKET listenSocket; // 서버에서 한번 정해지면 안바뀌는 값 (데이터레이스X, 전역해도 무방)

	int LISTEN_KEY = 999;

public:
	void mainServer(); // 메인 서버
	void error_display(const char* msg, int err_no);

	void recv_packet_construct(int user_id, int io_byte); // 수신한 패킷이 커서 짤려서 온 경우 재조립 함수
	void process_packet(int user_id, char* buf); // 패킷 처리 루틴
	void worker_thread();

	void send_packet(int user_id, void* packet); // 보낼 패킷의 버퍼와 사이즈 설정
	void send_login_ok_packet(int user_id); // 클라로 부터 accept 확인 시 클라 초기화 패킷 설정
	void send_move_packet(int user_id, int mover); // 변경된 위치값 설정
	void send_enter_packet(int user_id, int other_id);
	void send_leave_packet(int user_id, int other_id);

	
	void do_move(int user_id, char direction); // 클라에서 키 입력 받고 객체 움직이게 할때
	void enter_game(int user_id); // 다른 클라들 입장 알림

	void initalize_clients(); // 객체 연결 성공시 초기화
	void disconnect(int user_id);
};

