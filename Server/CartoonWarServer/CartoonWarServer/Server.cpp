#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::error_display(const char* msg, int err_no)
{
    WCHAR* lpMsgBuf; 
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM, 
        NULL, err_no, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    cout << msg;
    wcout << L"에러 " << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
}

void Server::recv_packet_construct(int user_id, int io_byte)
{
    int rest_byte = io_byte; // 이만큼 남았다, 이만큼 처리를 마저 해줘야한다

    char* p = g_clients[user_id].m_recv_over.io_buf;// 버퍼 중에서 어느 부분을 처리하고 있다, 패킷을 처리할수록 처리된 패킷 다음 데이터에 엑세스 해야함
    //우리가 처리해야할 데이터에 대한 포인터, 처음 시작하는거니까 io_buf에 들어있는 데이터 맨 앞부터 시작해야함
    int packet_size = 0;

    if (0 != g_clients[user_id].m_prev_size) // 이전에 받아놓은 데이터가 있을때
        packet_size = g_clients[user_id].m_packet_buf[0];

    while (rest_byte > 0) // 처리해야할 데이터가 남아있을때
    {
        if (0 == packet_size) // 지금 우리가 처리하는 패킷이 처음일때 -> 포인터를 패킷 데이터 맨 앞을 가리키게 설정
            packet_size = *p;

        if (packet_size <= rest_byte + g_clients[user_id].m_prev_size) // 지난번에 받은거랑 나머지랑 합쳐서 패킷 사이즈보다 크거나 같으면 패킷 완성
        {
            memcpy(g_clients[user_id].m_packet_buf + g_clients[user_id].m_prev_size, p, packet_size - g_clients[user_id].m_prev_size); // p에 있는걸 packet_size만큼 m_packet_buf에 복사
            
            p += packet_size - g_clients[user_id].m_prev_size;
            rest_byte -= packet_size - g_clients[user_id].m_prev_size;
            packet_size = 0;
            process_packet(user_id, g_clients[user_id].m_packet_buf);
            g_clients[user_id].m_prev_size = 0;
        }
        else // 합쳐도 패킷 사이브보다 작다, 패킷 완성을 못시킨다, 나머지를 저장해놓고 끝내야함
        {
            memcpy(g_clients[user_id].m_packet_buf + g_clients[user_id].m_prev_size, p, rest_byte); // p에 있는걸 rest_byte만큼 m_packet_buf에 복사
            // 혹시라도 2번이상 받았는데 패킷 완성 못시킨 경우가 생길 수 있으니 이전에 받아놓은 크기 뒤부터 복사해오게 설정
            g_clients[user_id].m_prev_size += rest_byte;
            rest_byte = 0;
            p += rest_byte; // 처리 해줬으니 그만큼 포인터 위치 이동
        }
    }
}

void Server::process_packet(int user_id, char* buf)
{
	switch (buf[1]) // 우리는 패킷을 0은 char size, 1은 char type으로 설정했으므로
	{
	case CS_PACKET_LOGIN: // case에서는 변수를 선언할때 중괄호 해줘야 변수로 선언이 된다
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
        enter_game(user_id, packet->name); // 새로 들어왔으니 접속 처리 및 이미 들어와있는 클라 정보 정리

        // 원래 락 걸때 socket이랑 connected랑 name을 동시에 보호해줘야함
        // bool connected 만든 이유가 true일때 socket이나 name처럼 한번 쓰고 업데이트 되지 않는
        // 값들은 안전하다는 의미, 그래서 connected를 true 되기 전에 socket, name을 처리하던가
        // true할때 같이 락을 걸던가 둘 중 하나로 해야함
	}
	break;
	case CS_PACKET_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
        g_clients[user_id].m_move_time = packet->move_time;
		do_move(user_id, packet->direction);
	}
	break;
	default:
		cout << "Unknown Packet Type Error\n";
		DebugBreak();
		exit(-1);
	}
}

void Server::send_login_ok_packet(int user_id)
{
	sc_packet_login_ok packet;
	packet.exp = 0;
	packet.hp = 0;
	packet.id = user_id;
	packet.level = 0;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.x = g_clients[user_id].m_x;
	packet.y = g_clients[user_id].m_y;

	send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_packet(int user_id, void* packet)
{
	char* buf = reinterpret_cast<char*>(packet);

	// m_recv_over는 recv 전용 오버랩 구조체이므로 쓰면 안된다
	// 그냥 OverEx overex로 오버랩 구조체 선언해서 쓰는것도 안된다, 로컬 변수라 함수 밖으로 나가면 사라져버림 -> 할당받아야함
	OverEx* overEx = new OverEx; // 확장 오버랩 할당
	overEx->function = FUNC_SEND; // 타입 설정
	ZeroMemory(&overEx->over, sizeof(overEx->over)); // 오버랩 구조체 초기화
	memcpy(overEx->io_buf, buf, buf[0]); // IOCP버퍼에 패킷 내용을 패킷 크기만큼 복사
	overEx->wsabuf.buf = overEx->io_buf; // WSA버퍼에 IOCP버퍼 복사
	overEx->wsabuf.len = buf[0]; // 버퍼 사이즈 설정

	WSASend(g_clients[user_id].m_socket, &overEx->wsabuf, 1, NULL, 0, &overEx->over, NULL);
}

void Server::do_move(int user_id, char direction)
{
    int x = g_clients[user_id].m_x;
    int y = g_clients[user_id].m_y;

    switch (direction)
    {
    case D_UP:
        if (y > 0)
            y--;
        break;

    case D_DOWN:
        if (y < WORLD_HEIGHT-1)
            y++;
        break;

    case D_LEFT:
        if (x > 0)
            x--;
        break;

    case D_RIGHT:
        if (x < WORLD_WIDTH - 1)
            x++;
        break;
    default:
        cout << "Unknown Direction From cs_move_packet !\n";
        DebugBreak();
        exit(-1);
    }
    g_clients[user_id].m_x = x;
    g_clients[user_id].m_y = y;

    for (auto &c : g_clients)
    {
        c.m_cLock.lock();
        if (ST_ACTIVE == c.m_status)
        {
            send_move_packet(c.m_id, user_id); // 연결된 모든 클라이언트들에게 움직인 클라의 위치값 전송
        }
        c.m_cLock.unlock();
    }
}

void Server::send_move_packet(int user_id, int mover)
{
    sc_packet_move packet;
    packet.id = mover;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;
    packet.x = g_clients[mover].m_x;
    packet.y = g_clients[mover].m_y; // 이동한 플레이어의 정보 담기
    packet.move_time = g_clients[mover].m_move_time;

    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::enter_game(int user_id, char name[])
{
    g_clients[user_id].m_cLock.lock();

    strcpy_s(g_clients[user_id].m_name, name);
    g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // 마지막에 NULL 넣어주는 처리
    send_login_ok_packet(user_id); // 새로 접속한 플레이어 초기화 정보 보내줌

    for (int i = 0; i < MAX_USER; i++) 
    {
        if (user_id == i)
            continue;
        g_clients[i].m_cLock.lock();
        if (ST_ACTIVE == g_clients[i].m_status) // 이미 연결 중인 클라들한테만
        {
            if (user_id != i) // 나 자신한텐 send_enter_packet 보낼 필요가 없음, 내가 들어왔다는걸 다른 클라에 알리는 패킷임
            {
                send_enter_packet(user_id, i); // 새로 접속한 클라에게 이미 연결중인 클라 정보들을 보냄 
                send_enter_packet(i, user_id); // 이미 접속한 플레이어들에게 새로 접속한 클라정보 보냄
            }
        }
        g_clients[i].m_cLock.unlock();
    }
    g_clients[user_id].m_cLock.unlock();
    // true == g_clients[i].m_isConnected 하므로 원래는 여기다 unlock 해야하는데 안에 범위가 너무 큼
    g_clients[user_id].m_status = ST_ACTIVE; // 다른 클라들한테 정보 보낸 다음에 마지막에 ST_ACTIVE로 바꿔주기
}

void Server::initalize_clients()
{
    for (int i = 0; i < MAX_USER; ++i)
    {
        g_clients[i].m_id = i; // 유저 등록
        g_clients[i].m_status = ST_FREE; // 여기는 멀티스레드 하기전에 싱글스레드일때 사용하는 함수, 락 불필요
    }
}

void Server::send_enter_packet(int user_id, int other_id)
{
    sc_packet_enter packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ENTER;
    packet.x = g_clients[other_id].m_x;
    packet.y = g_clients[other_id].m_y;
    strcpy_s(packet.name, g_clients[other_id].m_name);
    packet.o_type = O_PLAYER; // 다른 플레이어들의 정보 저장

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::disconnect(int user_id)
{
    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_status = ST_ALLOC; // 여기서 free 해버리면 아랫과정 진행중에 다른 클라에 할당될수도 있음
    send_leave_packet(user_id, user_id); // 나 자신
    closesocket(g_clients[user_id].m_socket);

    for (auto& c : g_clients) // 연결되어있는 클라이언트들에게 떠난 클라가 나갔다고 알림
    {
        if (user_id == c.m_id)
            continue;

        c.m_cLock.lock();
        if (ST_ACTIVE == c.m_status)
        {
            send_leave_packet(c.m_id, user_id);
        }
        c.m_cLock.unlock();
    }
    g_clients[user_id].m_status = ST_FREE; // 모든 처리가 끝내고 free해야함
    g_clients[user_id].m_cLock.unlock();
}

void Server::send_leave_packet(int user_id, int other_id)
{
    sc_packet_leave packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_LEAVE;

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::worker_thread()
{
    while (true)
    {
        DWORD io_byte;
        ULONG_PTR key;
        WSAOVERLAPPED* over;
        GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE); // recv 결과 IOCP에 저장
        // io_byte가 0인 경우 = 한 클라가 소켓을 종료했기 때문에 0바이트가 전송된다 
        OverEx* overEx = reinterpret_cast<OverEx*>(over); // 임시 확장 오버랩 구조체에 IOCP에 저장된 값 대입
        int id = static_cast<int>(key); // 임시 아이디에 IOCP 키값(클라 id값) 대입

        switch (overEx->function) // send, recv, accept 결정
        {
        case FUNC_RECV:
        {
            if (0 == io_byte)
                disconnect(id);
            else
            {
                recv_packet_construct(id, io_byte);
                ZeroMemory(&g_clients[id].m_recv_over.over, sizeof(g_clients[id].m_recv_over.over)); // 오버랩 구조체 초기화
                DWORD flags = 0;
                WSARecv(g_clients[id].m_socket, &g_clients[id].m_recv_over.wsabuf, 1, NULL, // 패킷 처리랑 초기화 끝나면 다시 recv 호출
                    &flags, &g_clients[id].m_recv_over.over, NULL);
            }
        }
        break;

        case FUNC_SEND:
            if (0 == io_byte)
                disconnect(id);

            delete overEx; // 확장 구조체 초기화만 해주면 된다
            break;

        case FUNC_ACCEPT:
        {
            // 현재는 한 쓰레드가 accept 끝나면 끝에 AcceptEx를 호출하게 해놔서 여러 쓰레드가 동시 접근 안함
            int user_id = -1;
            for (int i = 0; i < MAX_USER; ++i)
            {
                lock_guard <mutex> guardLock{ g_clients[i].m_cLock }; // 함수에서 lock 할때 편함
                // 이 cLock를 락을 걸고 락가드가 속한 블록에서 빠져나갈때 unlock해주고 루프 돌때마다 unlock-lock 해줌
                if (ST_FREE == g_clients[i].m_status) // 동접 객체 돌면서 새로 접속한애 id 부여
                {
                    g_clients[i].m_status = ST_ALLOC;
                    user_id = i;
                    break; // 할당 가능한 객체 있으면 break
                }
            }

            SOCKET clientSocket = overEx->c_socket;

            if (-1 == user_id) // 만약 모든 객체 돌았는데 할당 가능한 객체가 없을때
                closesocket(clientSocket); //send_login_fail_packet();
            else
            {
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, user_id, 0); // IOCP 객체에 소켓정도 등록 및 객체 초기화

                g_clients[user_id].m_prev_size = 0; // 버퍼 0으로 초기화
                g_clients[user_id].m_recv_over.function = FUNC_RECV; // 오버랩 구조체에 받는걸로 설정
                ZeroMemory(&g_clients[user_id].m_recv_over.over, sizeof(g_clients[user_id].m_recv_over.over)); // 오버랩 구조체 초기화
                g_clients[user_id].m_recv_over.wsabuf.buf = g_clients[user_id].m_recv_over.io_buf; // WSA 버퍼 위치 설정
                g_clients[user_id].m_recv_over.wsabuf.len = MAX_BUF_SIZE; // WSA버퍼 크기 설정
                g_clients[user_id].m_socket = clientSocket;
                g_clients[user_id].m_x = rand() % WORLD_WIDTH;
                g_clients[user_id].m_x = rand() % WORLD_HEIGHT;

                DWORD flags = 0;
                WSARecv(clientSocket, &g_clients[user_id].m_recv_over.wsabuf, 1, NULL,
                    &flags, &g_clients[user_id].m_recv_over.over, NULL); // 여기까지 하나의 클라 소켓 등록이랑 recv 호출이 끝났음
            }
            // 여기서부터 새로운 클라 소켓 accept
            clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // 새로 받을 클라 소켓 
            overEx->c_socket = clientSocket; // 새로 받을 클라니까 그 소켓 정보도 확장 오버랩에 넣어줘야함
            ZeroMemory(&overEx->over, sizeof(overEx->over)); // accept용 확장 오버랩 구조체 초기화
            // accept가 완료된 다음에 다시 accept 받는 부분이므로 overEx 다시 사용해도됨, 중복해서 불릴 일 없음
            AcceptEx(listenSocket, clientSocket, overEx->io_buf, NULL,
                sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &overEx->over);
        }
        break;
        }
    }
}

void Server::mainServer()
{
    wcout.imbue(std::locale("korean"));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // 그냥 바인드 쓰면 C++11에 있는 내장함수가 써짐
    listen(listenSocket, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // 커널 객체 생성, IOCP 객체 선언
    initalize_clients(); // 클라이언트 정보들 초기화

     // 비동기 accept의 완료를 받아야함 -> iocp로 받아야함 -> 리슨 소캣을 등록해줘야함
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), g_iocp, LISTEN_KEY, 0); // 리슨 소캣 iocp 객체에 등록
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OverEx accept_over;
    ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept용 확장 오버랩 구조체 초기화
    accept_over.function = FUNC_ACCEPT;
    accept_over.c_socket = clientSocket; // clientSocket을 worker_thread에 전달해줘야함
    // accept에선 wsabuf 이용안하므로 초기화 할 필요 없음
    AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);

    vector<thread> worker_threads;
    for (int i = 0; i < 4; ++i) // 여기에 쿼드코어라서 4 넣었는데 본인 코어수만큼 넣어도 ㄱㅊ
    {
        worker_threads.emplace_back([this]() {this->worker_thread(); });
    }

    for (auto& t : worker_threads)
    {
        t.join();
    }
    closesocket(listenSocket);
    WSACleanup();
}