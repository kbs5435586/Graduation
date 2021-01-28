#pragma once
enum ENUM_FUNC { FUNC_RECV, FUNC_SEND, FUNC_ACCEPT };

struct OverEx // 확장 오버랩 구조체
{
	WSAOVERLAPPED over; // 오버랩 구조체
	ENUM_FUNC function; // send, recv, accept 구별용
	char io_buf[MAX_BUF_SIZE]; // IOCP send-recv 버퍼
	union
	{
		WSABUF wsabuf;
		SOCKET c_socket;
	};
};

/*
OverEx에 그냥 SOCKET 자료형 변수를 추가해도 상관없는데
OverEx 자체가 엄청 자주 사용하는 자료구조이므로 오버헤드 발생 가능
union으로 WSABUF wsabuf; SOCKET c_socket; 선언하면 둘중 필요한거 하나만
꺼내쓸 수 있음
*/

struct ClientInfo // 클라이언트 정보
{
	SOCKET m_socket;
	int m_id;
	OverEx m_recv_over;
	int m_prev_size; // 잘린 파일의 경우 이전에 저장해둔 버퍼 크기
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv 성공시 저장해둘 버퍼
	bool m_isConnected; // 로그인 상태 판단용

	short m_x, m_y; // 나중에 맵이 256 범위 벗어날 수 있기 때문에 char로는 제한이 있음
	char m_name[MAX_ID_LEN + 1]; // +1은 아이디가 50 꽉차서 오면 안되긴 하지만 혹시라도 꽉 차서 왔을때 대비
};