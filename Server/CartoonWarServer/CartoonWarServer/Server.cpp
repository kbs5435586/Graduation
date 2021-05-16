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

void Server::start_game()
{
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
		do_move(user_id, packet->direction);
	}
	break;
    case CS_PACKET_ROTATE:
    {
        cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
        do_rotate(user_id, packet->direction);
    }
    break;
    case CS_PACKET_ADD_NPC:
    {
        cs_packet_add_npc* packet = reinterpret_cast<cs_packet_add_npc*>(buf);
        initialize_NPC(user_id);
    }
    break;
    case CS_PACKET_NPC_ACT:
    {
        cs_packet_npc_act* packet = reinterpret_cast<cs_packet_npc_act*>(buf);
        int p_id = packet->id;
        char p_act = packet->act;

        if (0 != g_clients[p_id].m_boid.size())
        {
            for (int i = 0; i < g_clients[p_id].m_boid.size(); i++)
            {
                if (ST_SLEEP == g_clients[p_id].m_boid[i]->m_status || ST_FREE == g_clients[p_id].m_boid[i]->m_status)
                {
                    continue;
                }
                else
                {
                    if (DO_ATTACK == p_act)
                    {
                        g_clients[p_id].m_boid[i]->m_last_order = FUNC_NPC_ATTACK;
                    }
                    if (DO_DEFENCE == p_act)
                    {
                        g_clients[p_id].m_boid[i]->m_last_order = FUNC_NPC_DEFENCE;
                    }
                    if (DO_HOLD == p_act)
                    {
                        g_clients[p_id].m_boid[i]->m_last_order = FUNC_NPC_HOLD;
                    }
                    if (DO_FOLLOW == p_act)
                    {
                        g_clients[p_id].m_boid[i]->m_last_order = FUNC_NPC_FOLLOW;
                    }
                    if (DO_RANDMOVE == p_act)
                    {
                        g_clients[p_id].m_boid[i]->m_last_order = FUNC_NPC_RANDMOVE;
                    }
                    activate_npc(i, g_clients[p_id].m_boid[i]->m_last_order);
                }
            }
        }
    }
    break;
    case CS_PACKET_CHANGE_FORMATION:
    {
        cs_packet_change_formation* packet = reinterpret_cast<cs_packet_change_formation*>(buf);
        do_change_formation(user_id);
    }
    break;
    case CS_PACKET_ATTACK:
    {
        cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(buf);
        do_attack(user_id);
    }
    break;
    case CS_PACKET_IDLE:
    {
        cs_packet_idle* packet = reinterpret_cast<cs_packet_idle*>(buf);
        do_idle(user_id);
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
	packet.hp = 100;
	packet.id = user_id;
	packet.level = 0;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
    _matrix pos = g_clients[user_id].m_transform.Get_Matrix();
    packet.r_x = pos._11;
    packet.r_y = pos._12;
    packet.r_z = pos._13;
    packet.u_x = pos._21;
    packet.u_y = pos._22;
    packet.u_z = pos._23;
    packet.l_x = pos._31;
    packet.l_y = pos._32;
    packet.l_z = pos._33;
    packet.p_x = pos._41;
    packet.p_y = pos._42;
    packet.p_z = pos._43;


	send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_flag_info_packet(int object_id, int user_id)
{
    sc_packet_flag_info packet;
    packet.id = object_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_FLAG_INFO;
    packet.isBlue = flags[object_id].isBlue;
    packet.isRed = flags[object_id].isRed;
    packet.p_x = flags[object_id].pos.x;
    packet.p_y = flags[object_id].pos.y;
    packet.p_z = flags[object_id].pos.z;

    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_flag_bool_packet(int object_id, int user_id)
{
    sc_packet_flag_bool packet;
    packet.id = object_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_FLAG_BOOL;
    packet.isBlue = flags[object_id].isBlue;
    packet.isRed = flags[object_id].isRed;

    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_time_packet()
{
    sc_packet_time packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_TIME;
    packet.time = play_time;

    for (int i = 0; i <= MAX_USER; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status) // 비접속 상태인 애들 무시
            continue;

        send_packet(i, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
    }
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
    _vec3* pos = g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    switch (direction)
    {
    case GO_UP:
        if (pos->y >= 0)
            pos->y--;
        break;
    case GO_DOWN:
        if (pos->y < WORLD_HEIGHT)
            pos->y++;
        break;
    case GO_LEFT:
        if (pos->x >= 0)
            g_clients[user_id].m_transform.Go_Left(MOVE_SPEED_PLAYER);
        break;
    case GO_RIGHT:
        if (pos->x < WORLD_HORIZONTAL)
            g_clients[user_id].m_transform.Go_Right(MOVE_SPEED_PLAYER);
        break;
    case GO_FORWARD:
        if (pos->z >= 0)
            g_clients[user_id].m_transform.BackWard(MOVE_SPEED_PLAYER);
        break;
    case GO_BACK:
        if (pos->z < WORLD_VERTICAL)
            g_clients[user_id].m_transform.Go_Straight(MOVE_SPEED_PLAYER);
        break;
    default:
        cout << "Unknown Direction From cs_move_packet !\n";
        DebugBreak();
        exit(-1);
    }

    if (pos->y < 0)
        pos->y = 0;
    if (pos->y >= (WORLD_HEIGHT - 1))
        pos->y = WORLD_HEIGHT - 1;
    if (pos->x < 0)
        pos->x = 0;
    if (pos->x >= (WORLD_HORIZONTAL - 1))
        pos->x = WORLD_HORIZONTAL - 1;
    if (pos->z >= (WORLD_VERTICAL - 1))
        pos->z = WORLD_VERTICAL - 1;
    if (pos->z < 0)
        pos->z = 0;

    g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, pos);

    is_flag_near(user_id);
    set_formation(user_id);

    g_clients[user_id].m_cLock.lock();
    unordered_set<int> old_viewlist = g_clients[user_id].m_view_list;
    // 복사본 뷰리스트에 다른 쓰레드가 접근하면 어쩌냐? 그 정도는 감수해야함
    g_clients[user_id].m_cLock.unlock();
    unordered_set<int> new_viewlist;

    for (auto& c : g_clients)
    {
        if (false == is_near(c.second.m_id, user_id)) // 근처에 없는애는 그냥 깨우지도 마라
            continue;
        //if (ST_SLEEP == c.second.m_status) // 근처에 있는 npc이면 깨워라
        //    activate_npc(c.second.m_id, c.second.m_last_order);
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (c.second.m_id == user_id)
            continue;
        //if (true == check_collision(c.second.m_id, user_id)) // 충돌을 한 놈이 있다면
        //{

        //}
        if (false == is_player(c.second.m_id)) // g_clients 객체가 플레이어가 아닌 npc이면
        {
            OverEx* overEx = new OverEx;
            overEx->function = FUNC_PLAYER_MOVE_FOR_NPC; // NPC에게 주변 플레이어가 움직였다는걸 알림
            overEx->player_id = user_id;
            PostQueuedCompletionStatus(g_iocp, 1, c.second.m_id, &overEx->over);
        }

        new_viewlist.insert(c.second.m_id); // 내 시야 범위안에 들어오는 다른 객체들의 아이디를 주입
    }

    // send_move_packet 해주는 부분
    send_move_packet(user_id, user_id); // 나한테 내가 이동한거 알려주는 용도

    for (auto new_vl : new_viewlist) // 움직인 이후의 시야 범위에 대하여
    {
        if (0 == old_viewlist.count(new_vl)) // 이전 뷰리스트에 new_vl의 개수가 0개 일때 = 이전 시야에 없던 애일때
        {
            send_enter_packet(user_id, new_vl); // 다른 객체들의 정보를 나에게 전송

            if (false == is_player(new_vl)) // 새로 시야에 들어온 애가 플레이어가 아니면 걍 반복문 넘김 / 이 아니라 npc 발동해주고 넘김
            {
                continue;
            }

            g_clients[new_vl].m_cLock.lock();
            if (0 == g_clients[new_vl].m_view_list.count(user_id)) // 상대의 뷰리스트에 내가 없다면
            {
                g_clients[new_vl].m_cLock.unlock();
                send_enter_packet(new_vl, user_id); // 나의 입장 정보를 다른 객체들에게 전송
            }
            else
            {
                g_clients[new_vl].m_cLock.unlock();
                send_move_packet(new_vl, user_id); // 나의 움직임 정보를 다른 객체들에게 전송
            }
        }
        else // 이동 한 후에 새 시야에 보이는 플레이어인데 이전에도 보였던 애다 = 기존 시야에 있던 애
        {
            if (false == is_player(new_vl)) // npc에게 내 새로운 움직임을 보내줄 필요가 없음
                continue;

            g_clients[new_vl].m_cLock.lock();
            if (0 != g_clients[new_vl].m_view_list.count(user_id))
            {
                g_clients[new_vl].m_cLock.unlock();
                send_move_packet(new_vl, user_id);
            }
            else
            {
                g_clients[new_vl].m_cLock.unlock();
                send_enter_packet(new_vl, user_id);
            }
        }
    }

    for (auto old_vl : old_viewlist) // 움직이기 전 시야범위에 대하여
    {
        if (0 == new_viewlist.count(old_vl)) // 새 시야범위에 old_vl 갯수가 0일때 = 시야 범위에서 벗어난 객체일때
        {
            send_leave_packet(user_id, old_vl); // 나에게 상대 객체가 나갔다 알림

            if (false == is_player(old_vl)) // npc에게 내가 나갔다는거 안알려도 된다
                continue;

            g_clients[old_vl].m_cLock.lock();
            if (0 != g_clients[old_vl].m_view_list.count(user_id))
            {
                g_clients[old_vl].m_cLock.unlock();
                send_leave_packet(old_vl, user_id); // 상대 객체에게 내가 나갔다 알림
            }
            else // 실수하기 쉬움, else에 뭐 없더라고 unlock 해줄것, 안그러면 조건 불만족시 락 안풀림
            {
                g_clients[old_vl].m_cLock.unlock();
            }
        }
    }
}

void Server::do_rotate(int user_id, char dir)
{
    if (TURN_RIGHT == dir)
        g_clients[user_id].m_transform.Rotation_Y(ROTATE_SPEED);
    else if (TURN_LEFT == dir)
        g_clients[user_id].m_transform.Rotation_Y(-ROTATE_SPEED);
    
    for (auto& c : g_clients) // 모든 객체랑 돌아간애 비교
    {
        if (false == is_near(c.second.m_id, user_id)) // 근처에 없는애면 보내지도 마라
            continue;
        //if (ST_SLEEP == c.second.m_status) // 근처에 있는 npc이면 깨워라
        //    activate_npc(c.second.m_id, c.second.m_last_order);
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (c.second.m_id == user_id)
            continue;
    }

    set_formation(user_id);

    g_clients[user_id].m_cLock.lock();
    unordered_set<int> copy_viewlist = g_clients[user_id].m_view_list;
    // 복사본 뷰리스트에 다른 쓰레드가 접근하면 어쩌냐? 그 정도는 감수해야함
    g_clients[user_id].m_cLock.unlock();

    send_rotate_packet(user_id, user_id); // 앞이 돌아갔다는 정보 받을애, 뒤에가 실제로 돌아간애, 일단 내가 나 돌아간거 알림
    for (auto cpy_vl : copy_viewlist) // 움직인 이후의 시야 범위에 대하여
    {
        send_rotate_packet(cpy_vl, user_id); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
        // 시야 범위 처리는 move 통해서만 하고 회전은 정보만 주고받으면 된다
    }
    do_npc_rotate(user_id, dir);
}

void Server::set_formation(int user_id)
{
    SESSION& c = g_clients[user_id];
    CTransform set_pos;
    _matrix temp = c.m_transform.Get_Matrix();

    switch (c.m_formation)
    {
    case FM_FLOCK:
    {
        if (1 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos;
        }
        else if (2 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;
        }
        else if (3 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;
        }
        else if (4 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;

            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos4 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[3]->m_target_pos = *new_pos4;
        }
    }
    break;
    case FM_SQUARE:
    {
        if (1 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos;
        }
        else if (2 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;
        }
        else if (3 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC * FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC * FORMATION_SPACE);
            set_pos.Go_Straight(MOVE_SPEED_NPC * FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;
        }
        else if (4 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC* FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC* FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC* FORMATION_SPACE);
            set_pos.BackWard(MOVE_SPEED_NPC* FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(MOVE_SPEED_NPC* FORMATION_SPACE);
            set_pos.Go_Straight(MOVE_SPEED_NPC* FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(MOVE_SPEED_NPC* FORMATION_SPACE);
            set_pos.Go_Straight(MOVE_SPEED_NPC* FORMATION_SPACE);
            _vec3* new_pos4 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[3]->m_target_pos = *new_pos4;
        }
    }
    break;
    case FM_PIRAMID:
    {

    }
    break;
    case FM_CIRCLE:
    {

    }
    break;
    }
}

void Server::do_random_move(int npc_id)
{
    _vec3* pos = g_clients[npc_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    switch (rand() % 4)
    {
    case MV_UP:
        if (pos->y > 0)
            pos->y--;
        else if (pos->y < 0)
            pos->y = 0;
        break;
    case MV_DOWN:
        if (pos->y < (WORLD_HEIGHT - 1))
            pos->y++;
        else if (pos->y > (WORLD_HEIGHT - 1))
            pos->y = WORLD_HEIGHT - 1;
        break;
    case MV_RIGHT: 
        if (pos->x < (WORLD_HORIZONTAL - 1))
            g_clients[npc_id].m_transform.Go_Right(MOVE_SPEED_NPC);
        else if (pos->x > (WORLD_HORIZONTAL - 1))
            pos->x = WORLD_HORIZONTAL - 1;
        break;
    case MV_LEFT:
        if (pos->x > 0)
            g_clients[npc_id].m_transform.Go_Left(MOVE_SPEED_NPC);
        else if (pos->x < 0)
            pos->x = 0;
        break;
    case MV_FORWARD:
        if (pos->z < (WORLD_VERTICAL - 1))
            g_clients[npc_id].m_transform.Go_Straight(MOVE_SPEED_NPC);
        else if (pos->z > (WORLD_VERTICAL - 1))
            pos->z = WORLD_VERTICAL - 1;
        break;
    case MV_BACK:
        if (pos->z > 0)
            g_clients[npc_id].m_transform.BackWard(MOVE_SPEED_NPC);
        else if (pos->z < 0)
            pos->z = 0;
        break;
    }

    g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, pos);

    for (int i = 0; i < NPC_START; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status)
            continue;

        if (true == is_near(i, npc_id))
        {
            g_clients[i].m_cLock.lock();
            if (0 != g_clients[i].m_view_list.count(npc_id))
            {
                g_clients[i].m_cLock.unlock();
                send_move_packet(i, npc_id);
            }
            else
            {
                g_clients[i].m_cLock.unlock();
                send_enter_packet(i, npc_id);
            }
        }
        else
        {
            g_clients[i].m_cLock.lock();
            if (0 != g_clients[i].m_view_list.count(npc_id))
            {
                g_clients[i].m_cLock.unlock(); // 여기 아마 잘못했을거임
                send_leave_packet(i, npc_id);
            }
            else
                g_clients[i].m_cLock.unlock();
        }
    }

    // 근데 이제 플레이어가 문제임, 플레이어 뷰 리스트 관리할때 npc까지 고려해서 뷰 리스트 관리해줘야 되므로 처음부터 끝까지 뷰리스트 다 살펴봐야함
}

void Server::do_follow(int npc_id)
{
    //bool isOnce = true;
    for (int i = 0; i < g_clients[g_clients[npc_id].m_owner_id].m_boid.size(); ++i)
    {
        if (g_clients[g_clients[npc_id].m_owner_id].m_boid[i]->m_id == g_clients[npc_id].m_id)
        {
            _vec3 Dir = move_to_spot(npc_id, &g_clients[g_clients[npc_id].m_owner_id].m_boid[i]->m_target_pos);
            _vec3* pos = g_clients[npc_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3 new_pos = *pos + Dir;
            if (*pos != new_pos)
            {
                //isOnce = false;
                g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &new_pos);

                for (int i = 0; i < NPC_START; ++i)
                {
                    if (ST_ACTIVE != g_clients[i].m_status)
                        continue;

                    if (true == is_near(i, npc_id))
                    {
                        g_clients[i].m_cLock.lock();
                        if (0 != g_clients[i].m_view_list.count(npc_id))
                        {
                            g_clients[i].m_cLock.unlock();
                            send_move_packet(i, npc_id);
                        }
                        else
                        {
                            g_clients[i].m_cLock.unlock();
                            send_enter_packet(i, npc_id);
                        }
                    }
                    else
                    {
                        g_clients[i].m_cLock.lock();
                        if (0 != g_clients[i].m_view_list.count(npc_id))
                        {
                            g_clients[i].m_cLock.unlock(); // 여기 아마 잘못했을거임
                            send_leave_packet(i, npc_id);
                        }
                        else
                            g_clients[i].m_cLock.unlock();
                    }
                }
                break;
            }
            else //if (*pos == new_pos && !isOnce)
            {
                do_idle(npc_id);
            }
        }
    }
}

void Server::do_change_formation(int player_id)
{
    g_clients[player_id].m_formation = ENUM_FORMATION(g_clients[player_id].m_formation + 1);
    if (FM_PIRAMID == g_clients[player_id].m_formation)
        g_clients[player_id].m_formation = FM_FLOCK;
    set_formation(player_id);
}

void Server::do_npc_rotate(int user_id, char dir)
{
    for (int i = MY_NPC_START(user_id); i <= MY_NPC_END(user_id); ++i)
    {
        if (ST_ACTIVE == g_clients[i].m_status)
        {
            if (TURN_RIGHT == dir)
                g_clients[i].m_transform.Rotation_Y(ROTATE_SPEED);
            else if (TURN_LEFT == dir)
                g_clients[i].m_transform.Rotation_Y(-ROTATE_SPEED);

            for (int player = 0; player < NPC_START; ++player)
            {
                if (ST_ACTIVE != g_clients[player].m_status)
                    continue;
                if (false == is_near(player, i)) // 근처에 없는애면 보내지도 마라
                    continue;

                send_rotate_packet(player, i); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
            }
        }
    }
}

_vec3 Server::move_to_spot(int id, _vec3* goto_pos)
{
    _vec3* new_pos = goto_pos;
    _vec3* now_pos = g_clients[id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3 Dir = { 0,0,0 };
    
    if (sqrt((new_pos->x - now_pos->x) *
        (new_pos->x - now_pos->x) +
        (new_pos->y - now_pos->y) *
        (new_pos->y - now_pos->y) +
        (new_pos->z - now_pos->z) *
        (new_pos->z - now_pos->z)) > NEAR_APPROACH)
    {
        Dir = *new_pos - *now_pos;
        float distance_square = Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z;
        if (0 != distance_square)
        {
            float hyp = sqrtf(Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);

            Dir = Dir / hyp; // 여기가 노멀값
            Dir = Dir * MOVE_SPEED_NPC; // 노멀값 방향으로 얼만큼 갈지 계산
        }
    }
    return Dir;
}

void Server::activate_npc(int npc_id, ENUM_FUNCTION op_type)
{
    ENUM_STATUS old_status = ST_SLEEP;
    if (true == atomic_compare_exchange_strong(&g_clients[npc_id].m_status, &old_status, ST_ACTIVE)) // m_status가 슬립에서 엑티브로 바뀐 경우에만
        // 동시에 두 클라가 접근하면 ACTIVE 로 2번 바뀌고 타이머가 2번 발동하는걸 방지하기 위한 용도
        add_timer(npc_id, op_type, FRAME_TIME);
}

void Server::event_player_move(int player_id, int npc_id)
{
    _vec3* p_pos = g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* n_pos = g_clients[npc_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    if (p_pos->x == n_pos->x)
    {
        if (p_pos->y == n_pos->y)
        {
            if (p_pos->z == n_pos->z)
            {
                char m[10] = "HELLO";
                send_chat_packet(player_id, npc_id, m);
            }
        }
    }
}

void Server::finite_state_machine(int npc_id, ENUM_FUNCTION func_id)
{
    // 하나의 NPC의 행동 관련된 것들을 작성할 것

    // 현재 상태 과거 상태 2개의 변수 비교해서 비교가 일어났을때만 상태머신 타게 구현
    // 커다란 과거 현재 비교하는 if문 내부터 switch 상태머신 돌리고 switch 끝나면
    // 과거는 = 현재 해주고 과거 현재 비교하는 if문 닫기
    // func_id 에 넣는게 새로운 상태, 그걸 과거 상태랑 비교 => 클라쪽에서 명령 자체를 못보내게 처리

    //bool keep_alive = false;
    //for (int i = 0; i < NPC_START; ++i) // 모든 플레이어에 대해서
    //{
    //    if (true == is_near(npc_id, i)) // 플레이어 시야범위 안에 있고
    //    {
    //        if (ST_ACTIVE == g_clients[i].m_status) // 접속해있는 플레이어일때
    //        {
    //            keep_alive = true; // 처음 시야범위 안에 들어온 플레이어 기준으로 npc가 활성화
    //            break;
    //        }
    //    }
    //}

    if (ST_ACTIVE == g_clients[g_clients[npc_id].m_owner_id].m_status) // NPC를 소유한 플레이어가 활성화 되어 있을때
    {
        switch (func_id)
        {
        case FUNC_NPC_ATTACK:
        {

        }
        break;
        case FUNC_NPC_DEFENCE:
        {

        }
        break;
        case FUNC_NPC_HOLD:
        {

        }
        break;
        case FUNC_NPC_FOLLOW:
        {
            do_follow(npc_id);
        }
        break;
        case FUNC_NPC_RANDMOVE:
        {
            do_random_move(npc_id);
        }
        break;
        }
    }
    if (FUNC_NPC_RANDMOVE == g_clients[npc_id].m_last_order)
        add_timer(npc_id, g_clients[npc_id].m_last_order, 1000); // 생성 이후 반복 간격
    else
        add_timer(npc_id, g_clients[npc_id].m_last_order, FRAME_TIME); // 생성 이후 반복 간격
}

void Server::add_timer(int obj_id, ENUM_FUNCTION op_type, int duration)
{
    timer_lock.lock();
    event_type event{ obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), 0 };
    timer_queue.push(event);
    //timer_queue.emplace(obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), 0);
    // 메모리 복사 안일어나게 하려고 위에 방식대로 했는데 emplace가 인자를 인식을 못함, 인자 개수가 많아질수록 이런버그 발생
    timer_lock.unlock();
}

void Server::do_timer()
{
    while (true)
    {
        //Sleep(1); // 윈도우에서만 가능함
        this_thread::sleep_for(1ms); // busy waiting 방지 겸 다른 쓰레드에서 cpu 양보, 1밀리초마다 검사해라, 계속 하고있지 말고
        while (true) // 실행 시간이 된게 있으면 계속 실행해주는 용
        {
            timer_lock.lock();
            if (true == timer_queue.empty()) // 타이머 큐에 아무것도 없으면
            {
                timer_lock.unlock();
                break; // 실행 시간이 안됐으면 루프 나가서 1초 쉬고옴
            }
            if (timer_queue.top().wakeup_time > high_resolution_clock::now()) // 현재 시간이 일어날시간보다 적으면 아직 일어날때가 아니다
                // 아직 큐에서 꺼낼때가 아니다           
            {
                // 근데 [ < busy waiting ]이렇게 돌려버리면 : 조건이 성립할 때까지 반복문을 실행하며 기다리는 방법> 발생함
                timer_lock.unlock();
                break; // 실행 시간이 안됐으면 루프 나가서 1초 쉬고옴
            }
            event_type event = timer_queue.top(); // 이렇게 하면 메모리 복사 일어남, 오버헤드 커짐, 그래서 큐의 자료형을 포인터로 받을것
            // 그렇게 해서 발생하는 new 자체가 오버헤드 아니냐, 맞음, 그래서 free list 써서 재사용 해줘야함 -> 알아서 할것
            timer_queue.pop();
            timer_lock.unlock();

            switch (event.event_id)
            {
            case FUNC_NPC_RANDMOVE:
            case FUNC_NPC_ATTACK:
            case FUNC_NPC_DEFENCE:
            case FUNC_NPC_HOLD:
            case FUNC_NPC_FOLLOW:
            case FUNC_CHECK_FLAG:
            case FUNC_CHECK_TIME:
            {
                OverEx* over = new OverEx;
                over->function = (ENUM_FUNCTION)event.event_id;
                PostQueuedCompletionStatus(g_iocp, 1, event.obj_id, &over->over);
                //random_move_npc(event.obj_id);
                //add_timer(event.obj_id, (ENUM_FUNCTION)event.event_id, 1000);
                // 타이머 쓰레드에서 움직이는거 처리까지 다 하면 과부화가 심하다
                // PostQueuedCompletionStatus 이걸로 worket thread에 작업 넘겨주고 여기선 어떤 이벤트인지만 알려줌
                // 오버랩 구조체 따로 초기화 안해줘도 되는게 PostQueuedCompletionStatus 자체가 진짜 넣어주는값 그대로 GetQueued에 넘겨줘서 괜찮음
                break;
            }
            }
        }
    }
}

void Server::send_move_packet(int user_id, int mover)
{
    sc_packet_move packet;
    packet.id = mover;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;
    _vec3* pos = g_clients[mover].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    packet.x = pos->x;  // 이동한 플레이어의 정보 담기
    packet.y = pos->y;
    packet.z = pos->z;

    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_rotate_packet(int user_id, int mover)
{
    sc_packet_rotate packet;
    packet.id = mover;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ROTATE;

    _vec3* r_pos = g_clients[mover].m_transform.Get_StateInfo(CTransform::STATE_RIGHT);
    packet.r_x = r_pos->x;
    packet.r_y = r_pos->y;
    packet.r_z = r_pos->z;

    _vec3* u_pos = g_clients[mover].m_transform.Get_StateInfo(CTransform::STATE_UP);
    packet.u_x = u_pos->x;
    packet.u_y = u_pos->y;
    packet.u_z = u_pos->z;

    _vec3* l_pos = g_clients[mover].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    packet.l_x = l_pos->x;
    packet.l_y = l_pos->y;
    packet.l_z = l_pos->z;

    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::enter_game(int user_id, char name[])
{
    g_clients[user_id].m_cLock.lock(); // name, m_status 락
    strcpy_s(g_clients[user_id].m_name, name);
    g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // 마지막에 NULL 넣어주는 처리
    send_login_ok_packet(user_id); // 새로 접속한 플레이어 초기화 정보 보내줌
    for (int i = 0; i < 5; ++i)
        send_flag_info_packet(i, user_id); // 새로 접속한 플레이어 초기화 정보 보내줌
    g_clients[user_id].m_status = ST_ACTIVE; // 다른 클라들한테 정보 보낸 다음에 마지막에 ST_ACTIVE로 바꿔주기
    g_clients[user_id].m_cLock.unlock();
    cout << "Player " << user_id << " login finish" << endl;
    for (int i = 0; i <= MAX_USER; ++i)
    {
        if (user_id == i) // 데드락 회피용
            continue;

        if (true == is_near(user_id, i))
        {
            //g_clients[i].m_cLock.lock();
            if (ST_SLEEP == g_clients[i].m_status)
            {
                activate_npc(i, FUNC_NPC_HOLD);
            }
            if (ST_ACTIVE == g_clients[i].m_status) // 이미 연결 중인 클라들한테만, m_status도 락을 걸어야 정상임
            {
                send_enter_packet(user_id, i); // 새로 접속한 클라에게 이미 연결중인 클라 정보들을 보냄 
                if (true == is_player(i))
                    send_enter_packet(i, user_id); // 이미 접속한 플레이어들에게 새로 접속한 클라정보 보냄
            }
            //g_clients[i].m_cLock.unlock();
        }
    }

    if (false == isGameStart)
    {
        short count = 0;
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE == g_clients[i].m_status)
            {
                count++;
            }

            if (StartGame_PlayerCount < count)
            {
                add_timer(-1, FUNC_CHECK_FLAG, 100);// 게임 플레이 시간 돌리는 함수
                add_timer(-1, FUNC_CHECK_TIME, 1000);// 게임 플레이 시간 돌리는 함수
                isGameStart = true;
                cout << "Game Routine Start!\n";
                break;
            }
        }
    }
}

void Server::initialize_clients()
{
    for (int i = 0; i < MAX_USER; ++i)
    {
        g_clients[i].m_id = i; // 유저 등록
        g_clients[i].m_status = ST_FREE; // 여기는 멀티스레드 하기전에 싱글스레드일때 사용하는 함수, 락 불필요
    }
}

void Server::initialize_objects()
{
    for (int i = 0; i < 5; ++i)
    {
        flags[i].isBlue = false;
        flags[i].isRed = false;
    }
    flags[0].pos = { 50.f, 0.2f, 50.f };
    flags[1].pos = { 100.f, 0.2f, 450.f };
    flags[2].pos = { 250.f, 0.2f, 250.f };
    flags[3].pos = { 450.f, 0.2f, 400.f };
    flags[4].pos = { 450.f, 0.2f, 100.f };
}

void Server::initialize_NPC(int player_id)
{
    for (int npc_id = MY_NPC_START(player_id); npc_id <= MY_NPC_END(player_id); npc_id++)
    {
        if (ST_ACTIVE != g_clients[npc_id].m_status)
        {
            g_clients[npc_id].m_socket = 0;
            g_clients[npc_id].m_id = npc_id;
            g_clients[npc_id].m_owner_id = player_id;
            g_clients[npc_id].m_last_order = FUNC_NPC_FOLLOW;
            g_clients[npc_id].m_team = g_clients[player_id].m_team;
            sprintf_s(g_clients[npc_id].m_name, "NPC %d", npc_id);
            g_clients[npc_id].m_status = ST_SLEEP;
            g_clients[npc_id].m_hp = 100;
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_UP,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_UP));
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_LOOK,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK));
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_RIGHT,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_RIGHT));
            g_clients[player_id].m_transform.Scaling(SCALE_X, SCALE_Y, SCALE_Z);
            g_clients[npc_id].m_speed = MOVE_SPEED_NPC;
            g_clients[player_id].m_boid.push_back(&g_clients[npc_id]);
            set_formation(player_id);
            for (int j = 0; j < g_clients[player_id].m_boid.size(); ++j)
            {
                if (g_clients[player_id].m_boid[j]->m_id == g_clients[npc_id].m_id)
                {
                    g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION,
                        &g_clients[player_id].m_boid[j]->m_target_pos);
                }
            }
            activate_npc(npc_id, g_clients[npc_id].m_last_order);
            for (int i = 0; i < NPC_START; ++i) // 다른 플레이어중에 시야범위 안에있는 플레이어에게 새로 생긴 npc 보이게하기
            {
                if (true == is_near(npc_id, i))
                {
                    if (ST_ACTIVE == g_clients[i].m_status) // 이미 연결 중인 클라들한테만, m_status도 락을 걸어야 정상임
                    {
                        if (true == is_player(i))
                            send_enter_packet(i, npc_id); // 이미 접속한 플레이어들에게 새로 접속한 클라정보 보냄
                    }
                    else
                        continue;
                }
            }
            break;
        }
        else
        {
            continue; // 여기 수정할것 (임시방편), 플레이어의 모든 npc active일때 더이상 추가 안되게 처리
        }
    }
}

void Server::send_enter_packet(int user_id, int other_id)
{
    sc_packet_enter packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ENTER;
    packet.hp = g_clients[other_id].m_hp;
    _matrix pos = g_clients[other_id].m_transform.Get_Matrix();
    packet.r_x = pos._11;
    packet.r_y = pos._12;
    packet.r_z = pos._13;
    packet.u_x = pos._21;
    packet.u_y = pos._22;
    packet.u_z = pos._23;
    packet.l_x = pos._31;
    packet.l_y = pos._32;
    packet.l_z = pos._33;
    packet.p_x = pos._41;
    packet.p_y = pos._42;
    packet.p_z = pos._43;

    strcpy_s(packet.name, g_clients[other_id].m_name);
    packet.o_type = O_HUMAN; // 다른 플레이어들의 정보 저장

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_attack_packet(int user_id, int other_id)
{
    sc_packet_attack packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ATTACK;
    packet.id = other_id;
    cout << user_id << " saw " << other_id << " attacking\n";
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_attacked_packet(int user_id, int other_id)
{
    sc_packet_attacked packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ATTACKED;
    packet.id = other_id;
    packet.hp = g_clients[other_id].m_hp;
    cout << user_id << " saw " << other_id << " attacked\n";
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_dead_packet(int user_id, int other_id)
{
    sc_packet_dead packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_DEAD;
    packet.id = g_clients[other_id].m_id;
    cout << user_id << " saw " << other_id << " dead\n";
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_leave_packet(int user_id, int other_id)
{
    sc_packet_leave packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_LEAVE;

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.erase(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_chat_packet(int listen_id, int chatter_id, char mess[])
{
    sc_packet_chat packet;
    packet.id = chatter_id; // 채팅 보내는 사람들의
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_CHAT;
    strcpy_s(packet.message, mess);

    send_packet(listen_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_npc_add_ok_packet(int user_id, int other_id)
{
    sc_packet_enter packet;
    packet.id = other_id; // 추가된 npc 아이디
    packet.hp = g_clients[other_id].m_hp;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ADD_NPC_OK;

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::do_idle(int user_id)
{
    if (user_id < NPC_START)
    {
        g_clients[user_id].m_cLock.lock();
        unordered_set<int> copy_viewlist = g_clients[user_id].m_view_list;
        g_clients[user_id].m_cLock.unlock();

        for (auto cpy_vl : copy_viewlist) // 움직인 이후의 시야 범위에 대하여
        {
            send_idle_packet(cpy_vl, user_id); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
            // 시야 범위 처리는 move 통해서만 하고 회전은 정보만 주고받으면 된다
        }
    }
    else if (user_id >= NPC_START && user_id <= MAX_NPC)
    {
        for (int i = 0; i < NPC_START; ++i) // 모든 플레이어에 대해서
        {
            if (false == is_near(i, user_id)) // 근처에 없는 유저면 보내지도 마라
                continue;
            if (ST_ACTIVE != g_clients[i].m_status) // 로그인 상태 아닌애면 보내지 마라
                continue;

            send_idle_packet(i, user_id); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
        }
    }
}

void Server::send_idle_packet(int user_id, int idler)
{
    sc_packet_idle packet;
    packet.id = idler;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_IDLE;
    cout << idler << " do idle\n";
    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}


void Server::do_attack(int user_id)
{
    cout << user_id << "is do attack\n";
    for (int i = 0; i < NPC_START; ++i) // 다른 플레이어들에게 내 공격모션 공유
    {
        if (false == is_near(i, user_id)) // 근처에 없는애들 무시
            continue;
        if (ST_ACTIVE != g_clients[i].m_status) // 비접속 상태인 애들 무시
            continue;
        if (i == user_id) // 나 자신 무시
            continue;

        send_attack_packet(i, user_id);
    }

    for (auto& c : g_clients)
    {
        if (false == is_near(c.second.m_id, user_id)) // 근처에 없는애들 무시
            continue;
        if (ST_ACTIVE != c.second.m_status) // 비접속 상태인 애들 무시
            continue;
        if (c.second.m_id == user_id) // 나 자신 무시
            continue;
        if (c.second.m_owner_id == user_id) // 내 npc들 무시
            continue;

        if (true == is_attackable(c.second.m_id, user_id)) // 내 공격 범위 안에 상대가 있으면
        {
            g_clients[c.second.m_id].m_cLock.lock();
            unordered_set<int> copy_viewlist = g_clients[c.second.m_id].m_view_list; // 죽은애의 시야범위 가져옴
            g_clients[c.second.m_id].m_cLock.unlock();

            c.second.m_hp -= ATTACK_DAMAGE;

            if (0 < c.second.m_hp)
            {
                send_attacked_packet(c.second.m_id, c.second.m_id); // 깎이고 남은 체력 있으면
                for (auto cpy_vl : copy_viewlist)
                {
                    send_attacked_packet(cpy_vl, c.second.m_id); // 깎이고 남은 체력 있으면
                }
            }
            else // 죽으면
            {
                g_clients[c.second.m_id].m_hp = 0;
                send_dead_packet(c.second.m_id, c.second.m_id); // 깎이고 남은 체력 있으면
                for (auto cpy_vl : copy_viewlist)
                    send_dead_packet(cpy_vl, c.second.m_id);
                    //send_leave_packet(cpy_vl, c.second.m_id); // 시야범위 애들한테 애 없앰
            }
        }
    }
}

void Server::disconnect(int user_id)
{
    send_leave_packet(user_id, user_id); // 나 자신
    cout << user_id << "is disconnect\n";
    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_status = ST_ALLOC; // 여기서 free 해버리면 아랫과정 진행중에 다른 클라에 할당될수도 있음
    closesocket(g_clients[user_id].m_socket);

    for (int i = MY_NPC_START(user_id); i <= MY_NPC_END(user_id); i++)
    {
        g_clients[i].m_last_order = FUNC_END;
        g_clients[i].m_status = ST_SLEEP;
    }

    for (int i = 0; i < NPC_START; ++i)
    {
        if (user_id == g_clients[i].m_id)
            continue;

        // c.second.m_cLock.lock();
        if (ST_ACTIVE == g_clients[i].m_status)
        {
            send_leave_packet(g_clients[i].m_id, user_id); // 어차피 send_leave_packet 내부에서 뷰리스트 삭제 해줘서 여기에 따로 할 필요X
        }
       // c.second.m_cLock.unlock();
    }
    g_clients[user_id].m_status = ST_FREE; // 모든 처리가 끝내고 free해야함
    g_clients[user_id].m_cLock.unlock();
}

bool Server::is_near(int a, int b)
{
    _vec3* a_pos = g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* b_pos = g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    if (sqrt((a_pos->x - b_pos->x) *
        (a_pos->x - b_pos->x) +
        (a_pos->y - b_pos->y) * 
        (a_pos->y - b_pos->y) +
        (a_pos->z - b_pos->z) *
        (a_pos->z - b_pos->z)) > VIEW_RADIUS)
        // abs = 절대값
        return false;
    // 이건 2D 게임이니까 모니터 기준으로 다 사각형이므로 사각형 기준으로 시야범위 계산
    // 3D 게임은 루트(x-x의 제곱 + y-y의 제곱)> VIEW_RADIUS 이면 false로 처리해야함
    return true;
}

bool Server::is_attackable(int a, int b)
{
    _vec3* a_pos = g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* b_pos = g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    if (sqrt((a_pos->x - b_pos->x) *
        (a_pos->x - b_pos->x) +
        (a_pos->y - b_pos->y) *
        (a_pos->y - b_pos->y) +
        (a_pos->z - b_pos->z) *
        (a_pos->z - b_pos->z)) > ATTACK_RADIUS)
        return false;
    return true;
}

bool Server::is_player(int id)
{
    return id < NPC_START;
}

void Server::is_flag_near(int flag)
{
    int count_red = 0;
    int count_blue = 0;

    for (int player = 0; player <= MAX_USER; ++player)
    {
        if (ST_ACTIVE != g_clients[player].m_status) // 비접속 상태인 애들 무시
            continue;

        _vec3* a_pos = &flags[flag].pos; // 깃발
        _vec3* b_pos = g_clients[player].m_transform.Get_StateInfo(CTransform::STATE_POSITION); // 플레이어

        if (sqrt((a_pos->x - b_pos->x) *
            (a_pos->x - b_pos->x) +
            (a_pos->y - b_pos->y) *
            (a_pos->y - b_pos->y) +
            (a_pos->z - b_pos->z) *
            (a_pos->z - b_pos->z)) < FLAG_RADIUS) // 깃발 범위 안이면
        {
            if (TEAM_RED == g_clients[player].m_team)
            {
                count_red++;
                if (false == flags[flag].isRed)
                {
                    flags[flag].isRed = true;
                    for (int j = 0; j <= MAX_USER; ++j)
                    {
                        if (ST_ACTIVE != g_clients[j].m_status) // 비접속 상태인 애들 무시
                            continue;

                        send_flag_bool_packet(flag, j);
                    }
                }

            }
            else if (TEAM_BLUE == g_clients[player].m_team)
            {
                count_blue++;
                if (false == flags[flag].isBlue)
                {
                    flags[flag].isRed = true;
                    for (int j = 0; j <= MAX_USER; ++j)
                    {
                        if (ST_ACTIVE != g_clients[j].m_status) // 비접속 상태인 애들 무시
                            continue;

                        send_flag_bool_packet(flag, j);
                    }
                }
            }
        }
    }

    if (0 == count_red)
    {
        if (true == flags[flag].isRed)
        {
            flags[flag].isRed = false;
            for (int j = 0; j <= MAX_USER; ++j)
            {
                if (ST_ACTIVE != g_clients[j].m_status) // 비접속 상태인 애들 무시
                    continue;

                send_flag_bool_packet(flag, j);
            }
        }
    }

    if (0 == count_blue)
    {
        if (true == flags[flag].isBlue)
        {
            flags[flag].isBlue = false;
            for (int j = 0; j <= MAX_USER; ++j)
            {
                if (ST_ACTIVE != g_clients[j].m_status) // 비접속 상태인 애들 무시
                    continue;

                send_flag_bool_packet(flag, j);
            }
        }
        
    }
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
                //g_clients[user_id].m_transform.Ready_Transform();
                _vec3 pos = { 10.f,0.f,0.f };
                g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);

                g_clients[user_id].m_transform.Rotation_Y(180 * (XM_PI / 180.0f));
                g_clients[user_id].m_transform.Scaling(SCALE_X, SCALE_Y, SCALE_Z);
                g_clients[user_id].m_speed = MOVE_SPEED_PLAYER;
                g_clients[user_id].m_hp = 100;
                g_clients[user_id].m_owner_id = user_id; // 유저 등록
                if (0 == user_id)
                    g_clients[user_id].m_team = TEAM_RED;
                else
                    g_clients[user_id].m_team = TEAM_BLUE;
                g_clients[user_id].m_last_order = FUNC_END;
                g_clients[user_id].m_formation = FM_FLOCK;
                g_clients[user_id].m_view_list.clear(); // 이전 뷰리스트 가지고 있으면 안되니 초기화

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
        case FUNC_PLAYER_MOVE_FOR_NPC: // API_Send_message 호출용
        {
            int npc_id = id;
            int player_id = overEx->player_id;
            event_player_move(player_id, npc_id);

            delete overEx;
        }
        break;
        case FUNC_NPC_ATTACK: // API_Send_message 호출용
            finite_state_machine(id, FUNC_NPC_ATTACK);
            delete overEx;
            break;
        case FUNC_NPC_DEFENCE: // API_Send_message 호출용
            finite_state_machine(id, FUNC_NPC_DEFENCE);
            delete overEx;
            break;
        case FUNC_NPC_HOLD: // API_Send_message 호출용
            finite_state_machine(id, FUNC_NPC_HOLD);
            delete overEx;
            break;
        case FUNC_NPC_FOLLOW: // API_Send_message 호출용
            finite_state_machine(id, FUNC_NPC_FOLLOW);
            delete overEx;
            break;
        case FUNC_NPC_RANDMOVE:
            finite_state_machine(id, FUNC_NPC_RANDMOVE);
            delete overEx;
            break;
        case FUNC_CHECK_FLAG:
            for (int i = 0; i < 5; ++i)
                is_flag_near(i);
            add_timer(-1, FUNC_CHECK_FLAG, 100);
            delete overEx;
            break;
        case FUNC_CHECK_TIME:
            if (play_time >= 0)
            {
                send_time_packet();
                play_time -= 1;
                add_timer(-1, FUNC_CHECK_TIME, 1000);
            }
            delete overEx;
            break;
        break;
        default:
            cout << "Unknown Operation in Worker_Thread\n";
            while (true);
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
    initialize_clients(); // 클라이언트 정보들 초기화
    initialize_objects(); // 오브젝트 정보들 초기화
    isGameStart = false;

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
    for (int i = 0; i < 6; ++i) // 여기에 쿼드코어라서 4 넣었는데 본인 코어수만큼 넣어도 ㄱㅊ
    {
        worker_threads.emplace_back([this]() {this->worker_thread(); });
    }

    //thread AI_thread([this]() {this->do_AI(); });
    //AI_thread.join();

    thread timer_thread([this]() {this->do_timer(); });

    for (auto& t : worker_threads)
    {
        t.join();
    }
    closesocket(listenSocket);
    WSACleanup();
}

//int Server::API_SendMessage(lua_State* L)
//{
//    int my_id = (int)lua_tointeger(L, -3);
//    int user_id = (int)lua_tointeger(L, -2);
//    char* mess = (char*)lua_tostring(L, -1);
//
//    send_chat_packet(user_id, my_id, mess);
//    lua_pop(L, 3);
//    return 0;
//}
//
//int Server::API_get_x(lua_State* L)
//{
//    int obj_id = (int)lua_tointeger(L, -1);
//    lua_pop(L, 2);
//    int x = g_clients[obj_id].m_x;
//    lua_pushnumber(L, x);
//    return 1;
//}
//
//int Server::API_get_y(lua_State* L)
//{
//    int obj_id = (int)lua_tointeger(L, -1);
//    lua_pop(L, 2);
//    int y = g_clients[obj_id].m_y;
//    lua_pushnumber(L, y);
//    return 1;
//}

bool Server::check_collision(int a, int b)
{
    _vec3* p1 = g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* p2 = g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    float* r1 = &g_clients[a].m_collision.sphere_r;
    float* r2 = &g_clients[b].m_collision.sphere_r;

    return fabs((p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y) + (p1->z - p2->z) * (p1->z - p2->z)
        <= (*r1 + *r2) * (*r1 + *r2));
}