#pragma once
enum ENUM_FUNCTION { FUNC_RECV, FUNC_SEND, FUNC_ACCEPT };
enum ENUM_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE };
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
	mutex m_cLock;
	SOCKET m_socket;
	int m_id;
	OverEx m_recv_over;
	int m_prev_size; // 잘린 파일의 경우 이전에 저장해둔 버퍼 크기
	char m_packet_buf[MAX_PACKET_SIZE]; // send, recv 성공시 저장해둘 버퍼
	ENUM_STATUS m_status;

	short m_x, m_y; // 나중에 맵이 256 범위 벗어날 수 있기 때문에 char로는 제한이 있음
	char m_name[MAX_ID_LEN + 1]; // +1은 아이디가 50 꽉차서 오면 안되긴 하지만 혹시라도 꽉 차서 왔을때 대비
	// m_isConnected가 true일때 m_name가 의미있음, true인데 m_name에 값이 없는 경우가 없어야함
	unsigned m_move_time;
};