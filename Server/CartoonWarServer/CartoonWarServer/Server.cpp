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
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    cout << msg;
    wcout << L"에러 " << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
}

const float Server::time_delta()
{
	QueryPerformanceCounter(&m_frameTime);

	float m_fTimeDelta = float(m_frameTime.QuadPart - m_lastTime.QuadPart) / m_CPUTick.QuadPart;

	m_lastTime = m_frameTime;

	if (m_frameTime.QuadPart - m_fixTime.QuadPart >= m_CPUTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CPUTick);
		m_fixTime = m_frameTime;
	}

	return m_fTimeDelta;
}

void Server::ready_timer()
{
    QueryPerformanceCounter(&m_frameTime);
    QueryPerformanceCounter(&m_fixTime);
    QueryPerformanceCounter(&m_lastTime);
    QueryPerformanceFrequency(&m_CPUTick);
    TIME_DELTA = 0.f;
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

int Server::init_arrow(int shoot_id)
{
    for (int i = OBJECT_START; i < MAX_OBJECT; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status)
        {
            g_clients[i].m_cLock.lock();
            g_clients[i].m_status = ST_ACTIVE;
            g_clients[i].m_cLock.unlock();

            g_clients[i].m_transform.Scaling(SCALE.x, SCALE.y, SCALE.z);
            g_clients[i].m_transform.SetUp_RotationY(XMConvertToRadians(-90.f));
            g_clients[i].m_move_speed = 100.f;
            g_clients[i].m_transform.SetUp_Speed(100.f, XMConvertToRadians(90.f));

            _matrix arrow = g_clients[i].m_transform.Get_Matrix();
            arrow *= g_clients[shoot_id].m_transform.Get_Matrix();
            g_clients[i].m_transform.Set_Matrix(&arrow);

            g_clients[i].m_type = TP_ARROW;
            g_clients[i].m_owner_id = shoot_id;
            g_clients[i].m_lifetime = high_resolution_clock::now();
            ARROW_COUNT++;
            add_timer(i, FUNC_ARROW, 1);
            return i;
        }
    }
    return -1;
}

void Server::do_move(int user_id, char direction)
{
    SESSION& p = g_clients[user_id];

    switch (direction)
    {
    case GO_UP:
        break;
    case GO_DOWN:
        break;
    case GO_LEFT:
    {
        _vec3 oldLook = *p.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
        oldLook = Vector3_::Normalize(oldLook);
        p.m_transform.Rotation_Y(-TIME_DELTA);
        _vec3 newLook = *p.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
        newLook = Vector3_::Normalize(newLook);

        float PdotProduct = Vector3_::DotProduct(oldLook, newLook); // 내각
        float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

        float PoutProduct = (oldLook.x * newLook.z) - (oldLook.z * newLook.x); // 앞에 x 벡터 기준 각도 차이
        if (PoutProduct > 0) // 양수이면 newLook는 oldLook로 부터 반시계
            radian *= -1.f;

        float angle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
        p.m_total_angle += angle;
    }
    break;
    case GO_RIGHT:
    {
        _vec3 oldLook = *p.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
        oldLook = Vector3_::Normalize(oldLook);
        p.m_transform.Rotation_Y(TIME_DELTA);
        _vec3 newLook = *p.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
        newLook = Vector3_::Normalize(newLook);

        float PdotProduct = Vector3_::DotProduct(oldLook, newLook); // 내각
        float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

        float PoutProduct = (oldLook.x * newLook.z) - (oldLook.z * newLook.x); // 앞에 x 벡터 기준 각도 차이
        if (PoutProduct > 0) // 양수이면 newLook는 oldLook로 부터 반시계
            radian *= -1.f;

        float angle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
        p.m_total_angle += angle;
    }
    break;
    case GO_FORWARD:
        p.m_transform.BackWard(TIME_DELTA);
        break;
    case GO_FAST_FORWARD:
        p.m_transform.BackWard(2.f * TIME_DELTA);
        break;
    case GO_BACK:
        p.m_transform.Go_Straight(TIME_DELTA);
        break;
    default:
        cout << "Unknown Direction From cs_move_packet !\n";
        DebugBreak();
        exit(-1);
    }

    _vec3* pos = p.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
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

    Update_Collider(user_id, p.m_col.aabb_size, COLLIDER_TYPE::COLLIDER_AABB);
    Update_Collider(user_id, p.m_col.obb_size, COLLIDER_TYPE::COLLIDER_OBB);
    Obb_Collision(user_id);

    for (auto& o : g_clients) // aabb 충돌체크
{
    if (o.second.m_id == user_id)
        continue;
    if (ST_ACTIVE != o.second.m_status)
        continue;
    if (ST_DEAD == o.second.m_status)
        continue;
    check_aabb_collision(user_id, o.second.m_id);
}

    set_formation(user_id);

    g_clients[user_id].m_cLock.lock();
    unordered_set<int> old_viewlist = g_clients[user_id].m_view_list;
    g_clients[user_id].m_cLock.unlock();
    unordered_set<int> new_viewlist;

    for (auto& c : g_clients)
    {
        if (false == is_near(c.second.m_id, user_id)) // 근처에 없는애는 그냥 깨우지도 마라
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (c.second.m_id == user_id)
            continue;
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

void Server::process_packet(int user_id, char* buf)
{
    switch (buf[1])
    {
    case CS_PACKET_LOGIN:
    {
        cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
        enter_game(user_id, packet->name);
    }
    break;
    case CS_PACKET_ADD_NPC:
    {
        cs_packet_add_npc* packet = reinterpret_cast<cs_packet_add_npc*>(buf);
        if (g_clients[user_id].m_gold > 0)
            initialize_NPC(user_id);
    }
    break;
    case CS_PACKET_NPC_ACT:
    {
        cs_packet_npc_act* packet = reinterpret_cast<cs_packet_npc_act*>(buf);
        do_change_npc_act(user_id, packet->act);
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
        g_clients[user_id].m_isAttack = true;

        for (int i = 0; i < OBJECT_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (user_id == i)
                continue;
            if (!is_attackable(i, user_id))
                continue;
            if (g_clients[i].m_team == g_clients[user_id].m_team)
                continue;
            if (dist_between(user_id, i) >= OBB_DIST)
                continue;
            if (check_obb_collision(user_id, i))
            {
                if (g_clients[user_id].m_isAttack)
                {
                    g_clients[i].m_isOBB = true;
                    g_clients[i].m_matAttackedTarget = g_clients[user_id].m_transform.Get_Matrix();
                    g_clients[user_id].m_isAttack = false;
                    g_clients[i].m_hp -= ATTACK_DAMAGE;
                    for (int player = 0; player < NPC_START; ++player)
                    {
                        if (!is_near(player, i))
                            continue;
                        send_do_particle_packet(player, i); // 남은 체력 브로드캐스팅
                        send_hp_packet(player, i);
                        send_animation_packet(player, i, A_HIT);
                    }
                }
            }
        }
    }
    break;
    case CS_PACKET_ANIMATION:
    {
        cs_packet_animation* packet = reinterpret_cast<cs_packet_animation*>(buf);
        do_animation(user_id, packet->anim);
    }
    break;
    case CS_PACKET_ARROW:
    {
        cs_packet_arrow* packet = reinterpret_cast<cs_packet_arrow*>(buf);
        int arrow_id = init_arrow(user_id);
        if (-1 == arrow_id)
        {
            cout << "no available arrow container\n";
            break;
        }
        else
        {
            for (int i = 0; i < NPC_START; ++i)
            {
                if (ST_ACTIVE != g_clients[i].m_status)
                    continue;
                send_arrow_packet(arrow_id, i, user_id);
            }
        }
    }
    break;
    case CS_PACKET_DEFFEND:
    {
        cs_packet_deffend* packet = reinterpret_cast<cs_packet_deffend*>(buf);
        for (int obj = OBJECT_START; obj < MAX_OBJECT; ++obj)
        {
            if (ST_ACTIVE != g_clients[obj].m_status)
            {
                g_clients[user_id].m_gold -= DEFFEND_PRICE;
                send_gold_packet(user_id);
                g_clients[obj].m_cLock.lock();
                g_clients[obj].m_status = ST_ACTIVE;
                g_clients[obj].m_cLock.unlock();
                g_clients[obj].m_id = obj;
                g_clients[obj].m_type = TP_DEFFEND;
                g_clients[obj].m_hp = SET_HP;

                _matrix own = g_clients[user_id].m_transform.Get_Matrix();
                
                CTransform Temp;
                Temp.Set_Matrix(&own);
                Temp.BackWard(10.f);
                Temp.Scaling(0.06f, 0.06f, 0.06f);
                own = Temp.Get_Matrix();
                g_clients[obj].m_transform.Set_Matrix(&own);
                
                //g_clients[obj].m_transform.SetUp_RotationY(XMConvertToRadians(-180.f));
                //g_clients[obj].m_transform.SetUp_Speed(100.f, XMConvertToRadians(90.f));
                g_clients[obj].m_col.aabb_size = { 250.f,250.f,250.f };
                g_clients[obj].m_col.obb_size = { 250.f,250.f,250.f };
                Ready_Collider_AABB_BOX(obj, g_clients[obj].m_col.aabb_size);
                Ready_Collider_OBB_BOX(obj, g_clients[obj].m_col.obb_size);
                g_clients[obj].m_isOBB = false;

                for (int i = 0; i < NPC_START; ++i)
                {
                    if (ST_ACTIVE != g_clients[i].m_status)
                        continue;
                    send_deffend_packet(obj, i, user_id);
                }
                break;
            }
        }
    }
    break;
    case CS_PACKET_TELEPORT:
    {
        cs_packet_teleport* packet = reinterpret_cast<cs_packet_teleport*>(buf);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            send_teleport_packet(i, packet->x, packet->z, M_ADD);
        }
    }
    break;
    case CS_PACKET_FIRE:
    {
        cs_packet_fire* packet = reinterpret_cast<cs_packet_fire*>(buf);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            send_fire_packet(i, packet->x, packet->z);
        }
        for (int i = OBJECT_START; i < MAX_OBJECT; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
            {
                g_clients[i].m_cLock.lock();
                g_clients[i].m_status = ST_ACTIVE;
                g_clients[i].m_cLock.unlock();
                _vec3 temp = { packet->x, 0.f,packet->z };
                g_clients[i].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &temp);
                g_clients[i].m_team = g_clients[user_id].m_team;
                g_clients[i].m_count = 0;
                do_dot_damage(i);
                break;
            }
        } 
    }
    break;
    case CS_PACKET_INVISIBLE:
    {
        cs_packet_invisible* packet = reinterpret_cast<cs_packet_invisible*>(buf);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            send_invisible_packet(i, user_id, packet->invisable);
        }
    }
    break;
    case CS_PACKET_POSITION:
    {
        cs_packet_position* packet = reinterpret_cast<cs_packet_position*>(buf);
        _vec3 pos;
        pos.x = packet->x;
        pos.y = packet->y;
        pos.z = packet->z;
        g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
    }
    break;
    case CS_PACKET_MOVE:
    {
        cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
        //cout << "recv move\n";
        do_move(user_id, packet->dir);
    }
    break;
    case CS_PACKET_TROOP_CHANGE:
    {
        cs_packet_change_troop* packet = reinterpret_cast<cs_packet_change_troop*>(buf);
        g_clients[user_id].m_troop = packet->troop;
    }
    break;
    case CS_PACKET_CLASS_CHANGE:
    {
        cs_packet_class_change* packet = reinterpret_cast<cs_packet_class_change*>(buf);
        int recv_id = packet->id;
        g_clients[recv_id].m_class = packet->p_class;
        
        if (!is_player(recv_id))
        {
            cal_change_class_gold(g_clients[recv_id].m_owner_id, packet->p_class);
            update_npc_troop(recv_id);
        }
        else
            cal_change_class_gold(recv_id, packet->p_class);

        update_speed_and_collider(recv_id);

        for (int i = 0; i < NPC_START; ++i) // npc 시야범위 내 있는 플레이어들에게 신호 보내는 곳
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (true == is_near(recv_id, i))
            {
                send_class_change_packet(i, recv_id);
            }
        }
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
    packet.p_class = g_clients[user_id].m_class;
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
        if (ST_ACTIVE != g_clients[i].m_status)
            continue;
        send_packet(i, &packet);
    }
}

void Server::send_time_delta(int user_id, float time)
{
    sc_packet_timedelta packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_TIMEDELTA;
    packet.time = time;

    send_packet(user_id, &packet);
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

void Server::update_npc_troop(int npc_id)
{
    SESSION& c = g_clients[npc_id];

    if (C_WORKER == c.m_class)
        c.m_troop = T_INFT;
    if (C_CAVALRY == c.m_class)
        c.m_troop = T_HORSE;
    if (C_TWO == c.m_class)
        c.m_troop = T_INFT;
    if (C_INFANTRY == c.m_class)
        c.m_troop = T_INFT;
    if (C_FOUR == c.m_class)
        c.m_troop = T_HORSE;
    if (C_SPEARMAN == c.m_class)
        c.m_troop = T_INFT;
    if (C_MAGE == c.m_class)
        c.m_troop = T_MAGE;
    if (C_MMAGE == c.m_class)
        c.m_troop = T_HORSE;
    if (C_ARCHER == c.m_class)
        c.m_troop = T_BOW;
}

void Server::update_speed_and_collider(int user_id)
{
    SESSION& c = g_clients[user_id];

    if (C_WORKER == c.m_class)
    {
        c.m_move_speed = 10.f;
        c.m_col.aabb_size = { 20.f ,80.f,20.f }; // aabb
        c.m_col.obb_size = { 30.f ,80.f,30.f }; // obb
    }
    if (C_CAVALRY == c.m_class)
    {
        c.m_move_speed = 20.f;
        c.m_col.aabb_size = { 40.f ,80.f,40.f };
        c.m_col.obb_size = { 80.f ,80.f,80.f };
    }
    if (C_TWO == c.m_class)
    {
        c.m_move_speed = 20.f;
        c.m_col.aabb_size = { 40.f ,80.f,40.f };
        c.m_col.obb_size = { 80.f ,80.f,80.f };
    }
    if (C_INFANTRY == c.m_class)
    {
        c.m_move_speed = 10.f;
        c.m_col.aabb_size = { 10.f ,80.f,10.f };
        c.m_col.obb_size = { 30.f ,80.f,30.f };
    }
    if (C_FOUR == c.m_class)
    {
        c.m_move_speed = 10.f;
        c.m_col.aabb_size = { 10.f ,80.f,10.f };
        c.m_col.obb_size = { 30.f ,80.f,30.f };
    }
    if (C_SPEARMAN == c.m_class)
    {
        c.m_move_speed = 10.f;
        c.m_col.aabb_size = { 20.f ,80.f,20.f };
        c.m_col.obb_size = { 100.f ,80.f,100.f };
    }
    if (C_MAGE == c.m_class)
    {
        c.m_move_speed = 10.f;
        c.m_col.aabb_size = { 20.f ,80.f,20.f };
        c.m_col.obb_size = { 30.f ,80.f,30.f };
    }
    if (C_MMAGE == c.m_class)
    {
        c.m_move_speed = 20.f;
        c.m_col.aabb_size = { 20.f ,80.f,60.f };
        c.m_col.obb_size = { 80.f ,80.f,80.f };
    }
    if (C_ARCHER == c.m_class)
    {
        c.m_move_speed = 15.f;
        c.m_col.aabb_size = { 20.f ,80.f,20.f };
        c.m_col.obb_size = { 20.f ,80.f,20.f };
    }
}

void Server::set_formation(int user_id)
{
    SESSION& c = g_clients[user_id];
    _vec3 playerPos = *c.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3 set_pos = {};

    switch (c.m_formation)

    {
    case F_SQUARE:
    {
        if (1 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;
        }
        else if (2 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;
        }
        else if (3 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;
        }
        else if (4 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 135.f) * (PIE / 180.f));
            _vec3 new_pos3 = playerPos + set_pos;
            c.m_boid[3].final_pos = new_pos3;
            c.m_boid[3].angle = c.m_total_angle + 135.f;
            c.m_boid[3].radius = BASIC_FORM_RAD;
        }
        else if (5 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 135.f) * (PIE / 180.f));
            _vec3 new_pos3 = playerPos + set_pos;
            c.m_boid[3].final_pos = new_pos3;
            c.m_boid[3].angle = c.m_total_angle + 135.f;
            c.m_boid[3].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 75.f) * (PIE / 180.f));
            _vec3 new_pos4 = playerPos + set_pos;
            c.m_boid[4].final_pos = new_pos4;
            c.m_boid[4].angle = c.m_total_angle - 75.f;
            c.m_boid[4].radius = BASIC_FORM_RAD * 2;
        }
        else if (6 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 135.f) * (PIE / 180.f));
            _vec3 new_pos3 = playerPos + set_pos;
            c.m_boid[3].final_pos = new_pos3;
            c.m_boid[3].angle = c.m_total_angle + 135.f;
            c.m_boid[3].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 75.f) * (PIE / 180.f));
            _vec3 new_pos4 = playerPos + set_pos;
            c.m_boid[4].final_pos = new_pos4;
            c.m_boid[4].angle = c.m_total_angle - 75.f;
            c.m_boid[4].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle + 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle + 75.f) * (PIE / 180.f));
            _vec3 new_pos5 = playerPos + set_pos;
            c.m_boid[5].final_pos = new_pos5;
            c.m_boid[5].angle = c.m_total_angle + 75.f;
            c.m_boid[5].radius = BASIC_FORM_RAD * 2;
        }
        else if (7 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 135.f) * (PIE / 180.f));
            _vec3 new_pos3 = playerPos + set_pos;
            c.m_boid[3].final_pos = new_pos3;
            c.m_boid[3].angle = c.m_total_angle + 135.f;
            c.m_boid[3].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 75.f) * (PIE / 180.f));
            _vec3 new_pos4 = playerPos + set_pos;
            c.m_boid[4].final_pos = new_pos4;
            c.m_boid[4].angle = c.m_total_angle - 75.f;
            c.m_boid[4].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle + 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle + 75.f) * (PIE / 180.f));
            _vec3 new_pos5 = playerPos + set_pos;
            c.m_boid[5].final_pos = new_pos5;
            c.m_boid[5].angle = c.m_total_angle + 75.f;
            c.m_boid[5].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 105.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 105.f) * (PIE / 180.f));
            _vec3 new_pos6 = playerPos + set_pos;
            c.m_boid[6].final_pos = new_pos6;
            c.m_boid[6].angle = c.m_total_angle - 105.f;
            c.m_boid[6].radius = BASIC_FORM_RAD * 2;
        }
        else if (8 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 135.f) * (PIE / 180.f));
            _vec3 new_pos3 = playerPos + set_pos;
            c.m_boid[3].final_pos = new_pos3;
            c.m_boid[3].angle = c.m_total_angle + 135.f;
            c.m_boid[3].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 75.f) * (PIE / 180.f));
            _vec3 new_pos4 = playerPos + set_pos;
            c.m_boid[4].final_pos = new_pos4;
            c.m_boid[4].angle = c.m_total_angle - 75.f;
            c.m_boid[4].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle + 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle + 75.f) * (PIE / 180.f));
            _vec3 new_pos5 = playerPos + set_pos;
            c.m_boid[5].final_pos = new_pos5;
            c.m_boid[5].angle = c.m_total_angle + 75.f;
            c.m_boid[5].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 105.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 105.f) * (PIE / 180.f));
            _vec3 new_pos6 = playerPos + set_pos;
            c.m_boid[6].final_pos = new_pos6;
            c.m_boid[6].angle = c.m_total_angle - 105.f;
            c.m_boid[6].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle + 105.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle + 105.f) * (PIE / 180.f));
            _vec3 new_pos7 = playerPos + set_pos;
            c.m_boid[7].final_pos = new_pos7;
            c.m_boid[7].angle = c.m_total_angle + 105.f;
            c.m_boid[7].radius = BASIC_FORM_RAD * 2;
        }
        else if (9 == c.m_boid.size())
        {
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 45.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0].final_pos = new_pos;
            c.m_boid[0].angle = c.m_total_angle - 45.f;
            c.m_boid[0].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 45.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 45.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1].final_pos = new_pos1;
            c.m_boid[1].angle = c.m_total_angle + 45.f;
            c.m_boid[1].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle - 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle - 135.f) * (PIE / 180.f));
            _vec3 new_pos2 = playerPos + set_pos;
            c.m_boid[2].final_pos = new_pos2;
            c.m_boid[2].angle = c.m_total_angle - 135.f;
            c.m_boid[2].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * sinf((c.m_total_angle + 135.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * cosf((c.m_total_angle + 135.f) * (PIE / 180.f));
            _vec3 new_pos3 = playerPos + set_pos;
            c.m_boid[3].final_pos = new_pos3;
            c.m_boid[3].angle = c.m_total_angle + 135.f;
            c.m_boid[3].radius = BASIC_FORM_RAD;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 75.f) * (PIE / 180.f));
            _vec3 new_pos4 = playerPos + set_pos;
            c.m_boid[4].final_pos = new_pos4;
            c.m_boid[4].angle = c.m_total_angle - 75.f;
            c.m_boid[4].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle + 75.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle + 75.f) * (PIE / 180.f));
            _vec3 new_pos5 = playerPos + set_pos;
            c.m_boid[5].final_pos = new_pos5;
            c.m_boid[5].angle = c.m_total_angle + 75.f;
            c.m_boid[5].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle - 105.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle - 105.f) * (PIE / 180.f));
            _vec3 new_pos6 = playerPos + set_pos;
            c.m_boid[6].final_pos = new_pos6;
            c.m_boid[6].angle = c.m_total_angle - 105.f;
            c.m_boid[6].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = BASIC_FORM_RAD * 2 * sinf((c.m_total_angle + 105.f) * (PIE / 180.f));
            set_pos.z = BASIC_FORM_RAD * 2 * cosf((c.m_total_angle + 105.f) * (PIE / 180.f));
            _vec3 new_pos7 = playerPos + set_pos;
            c.m_boid[7].final_pos = new_pos7;
            c.m_boid[7].angle = c.m_total_angle + 105.f;
            c.m_boid[7].radius = BASIC_FORM_RAD * 2;

            set_pos = {};
            set_pos.x = 3 * BASIC_FORM_RAD / 2 * sinf((c.m_total_angle) * (PIE / 180.f));
            set_pos.z = 3 * BASIC_FORM_RAD / 2 * cosf((c.m_total_angle) * (PIE / 180.f));
            _vec3 new_pos8 = playerPos + set_pos;
            c.m_boid[8].final_pos = new_pos8;
            c.m_boid[8].angle = c.m_total_angle;
            c.m_boid[8].radius = 3 * BASIC_FORM_RAD / 2;
        }
    }
    break;
    }
}

void Server::do_follow(int npc_id)
{
    SESSION& n = g_clients[npc_id];
    for (int i = 0; i < g_clients[n.m_owner_id].m_boid.size(); ++i)
    {
        if (g_clients[n.m_owner_id].m_boid[i].id == n.m_id)
        {
            _vec3 p_pos = *g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3 p_look = *g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
            _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3 n_look = *n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
            if (n_pos != g_clients[n.m_owner_id].m_boid[i].final_pos) // 자신이 마지막 위치가 아닐때
            {
                n.m_anim = A_IDLE;
                if (dist_between(npc_id, n.m_owner_id) > g_clients[n.m_owner_id].m_boid[i].radius + NEAR_APPROACH
                    || dist_between(npc_id, n.m_owner_id) < g_clients[n.m_owner_id].m_boid[i].radius - NEAR_APPROACH) // 반지름 반경 안이나 밖일때
                {
                    n.m_anim = A_WALK;
                    _vec3 Dir = move_to_spot(npc_id, &g_clients[n.m_owner_id].m_boid[i].final_pos);
                    _vec3 new_pos = n_pos + Dir;
                    n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &new_pos);

                    _vec3 t_pos = g_clients[n.m_owner_id].m_boid[i].final_pos; // 자기가 가야할 목표위치를 안바라볼때
                    _vec3 t_look = Vector3_::Subtract(t_pos, n_pos);
                    t_look = Vector3_::Normalize(t_look);

                    float PdotProduct = Vector3_::DotProduct(n_look, t_look);
                    float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

                    float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // 앞에 x 벡터 기준 각도 차이
                    if (PoutProduct > 0) // 양수이면 n_look는 t_look로 부터 반시계
                        radian *= -1.f;

                    float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 목표위치 기준 몇도 차이나는지

                    if (NPCangle > 2.0f || NPCangle < -2.0f) // npc가 목표위치를 안바라볼때
                    {
                        n.m_anim = A_WALK;
                        if (NPCangle > 2.f)
                        {
                            n.m_transform.Rotation_Y(TIME_DELTA * 2.f);
                        }
                        else if (NPCangle < -2.f)
                        {
                            n.m_transform.Rotation_Y(-TIME_DELTA * 2.f);
                        }
                    }
                    n.m_isOut = true;
                }
                else
                {
                    if (n.m_isOut) // 이제 막 처음 반지름 반경에 들어왔을때
                    {
                        n.m_isOut = false;
                        _vec3 p_look = *g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
                        _vec3 standard = -1.f * Vector3_::Normalize(p_look);
                        _vec3 npcLookAt = n_pos - p_pos;
                        npcLookAt = Vector3_::Normalize(npcLookAt);

                        _vec3 set_pos = {};

                        float PdotProduct = (npcLookAt.x * standard.x) + (npcLookAt.y * standard.y) + (npcLookAt.z * standard.z);
                        _vec3 PoutProduct;
                        PoutProduct.x = (standard.y * npcLookAt.z) - (standard.z * npcLookAt.y);
                        PoutProduct.y = (standard.z * npcLookAt.x) - (standard.x * npcLookAt.z);
                        PoutProduct.z = (standard.x * npcLookAt.y) - (standard.y * npcLookAt.x);

                        float radian = acosf(PdotProduct);
                        if (PoutProduct.y < 0)
                            radian *= -1.f;
                        float NPCangle = radian * 180.f / PIE;
                        n.m_total_angle = g_clients[n.m_owner_id].m_total_angle + NPCangle;
                        n_pos.x = g_clients[n.m_owner_id].m_boid[i].radius * sinf((n.m_total_angle) * (PIE / 180.f)) + p_pos.x;
                        n_pos.z = g_clients[n.m_owner_id].m_boid[i].radius * cosf((n.m_total_angle) * (PIE / 180.f)) + p_pos.z;
                        n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &n_pos);
                    }
                    else // 이미 반지름 반경에 있던 애일때
                    {
                        if (g_clients[n.m_owner_id].m_boid[i].angle + 3.f < n.m_total_angle // 포지션 회전각도가 아닐때
                            || g_clients[n.m_owner_id].m_boid[i].angle - 3.f > n.m_total_angle)
                        {
                            n.m_anim = A_WALK;
                            if (g_clients[n.m_owner_id].m_boid[i].angle > n.m_total_angle)
                            {
                                n.m_total_angle += 1.5f;
                                n_pos.x = g_clients[n.m_owner_id].m_boid[i].radius * sinf((n.m_total_angle) * (PIE / 180.f)) + p_pos.x;
                                n_pos.z = g_clients[n.m_owner_id].m_boid[i].radius * cosf((n.m_total_angle) * (PIE / 180.f)) + p_pos.z;
                                n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &n_pos);
                            }
                            else
                            {
                                n.m_total_angle -= 1.5f;
                                n_pos.x = g_clients[n.m_owner_id].m_boid[i].radius * sinf((n.m_total_angle) * (PIE / 180.f)) + p_pos.x;
                                n_pos.z = g_clients[n.m_owner_id].m_boid[i].radius * cosf((n.m_total_angle) * (PIE / 180.f)) + p_pos.z;
                                n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &n_pos);
                            }
                        }
                        else // 포지션 회전각도 일때
                        {
                            _vec3 Dir = move_to_spot(npc_id, &g_clients[n.m_owner_id].m_boid[i].final_pos);
                            _vec3 new_pos = n_pos + Dir;
                            n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &new_pos);
                        }

                        n_look = Vector3_::Normalize(n_look);
                        p_look = Vector3_::Normalize(p_look);
                        float PdotProduct = Vector3_::DotProduct(n_look, p_look);
                        float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

                        float PoutProduct = (p_look.x * n_look.z) - (p_look.z * n_look.x); // 앞에 x 벡터 기준 각도 차이
                        if (PoutProduct > 0) // 양수이면 n_look는 t_look로 부터 반시계
                            radian *= -1.f;

                        float NPCangle = radian * 180.f / PIE; // 현재 npc look이 player look 기준 몇도 차이나는지

                        if (NPCangle > 2.f || NPCangle < -2.f) // npc가 look를 안바라볼때
                        {
                            n.m_anim = A_WALK;
                            if (NPCangle > 2.f)
                            {
                                n.m_transform.Rotation_Y(-TIME_DELTA * 2.f);
                            }
                            else if (NPCangle < -2.f)
                            {
                                n.m_transform.Rotation_Y(TIME_DELTA * 2.f);
                            }
                        }
                    }
                }
            }
            else // 자신의 마지막 위치일때
                n.m_anim = A_IDLE;
        }
    }
}

void Server::do_change_formation(int player_id)
{
    if (F_SQUARE == g_clients[player_id].m_formation)
        g_clients[player_id].m_formation = F_PIRAMID;
    else if (F_PIRAMID == g_clients[player_id].m_formation)
        g_clients[player_id].m_formation = F_CIRCLE;
    else if (F_CIRCLE == g_clients[player_id].m_formation)
        g_clients[player_id].m_formation = F_FLOCK;
    else  if (F_FLOCK == g_clients[player_id].m_formation)
        g_clients[player_id].m_formation = F_SQUARE;

    set_formation(player_id);
}

float Server::dist_between(int user_id, int other_id)
{
    _vec3 pos1 = *g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3 pos2 = *g_clients[other_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    _vec3 vLenTemp = Vector3_::Subtract(pos1, pos2);
    _float fLen = vLenTemp.Length();

    return fLen;
}

float Server::dist_between_finalPos(int user_id, int i)
{
    _vec3* pos1 = g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3 finalPos = g_clients[user_id].m_boid[i].final_pos;

    float dist = sqrt((pos1->x - finalPos.x) * (pos1->x - finalPos.x) + (pos1->y - finalPos.y) * (pos1->y - finalPos.y)
        + (pos1->z - finalPos.z) * (pos1->z - finalPos.z));

    return dist;
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
            Dir = Vector3_::Normalize(Dir);
            Dir = Dir * g_clients[id].m_move_speed * TIME_DELTA * 2.f; // 노멀값 방향으로 얼만큼 갈지 계산
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

void Server::finite_state_machine(int npc_id, ENUM_FUNCTION func_id)
{
    SESSION& n = g_clients[npc_id];

    if (ST_ACTIVE == g_clients[n.m_owner_id].m_status && ST_ACTIVE == g_clients[npc_id].m_status) // NPC를 소유한 플레이어가 활성화 되어 있을때
    {
        switch (func_id)
        {
        case FUNC_NPC_ATTACK:
        {
            do_attack(npc_id);
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
        }
    }

    Update_Collider(npc_id, n.m_col.aabb_size, COLLIDER_TYPE::COLLIDER_AABB);
    Update_Collider(npc_id, n.m_col.obb_size, COLLIDER_TYPE::COLLIDER_OBB);
    Obb_Collision(npc_id);

    for (auto& o : g_clients) // aabb 충돌체크
    {
        if (o.second.m_id == npc_id)
            continue;
        if (ST_ACTIVE != o.second.m_status)
            continue;
        if (ST_DEAD == o.second.m_status)
            continue;
        check_aabb_collision(npc_id, o.second.m_id);
    }

    for (int i = 0; i < NPC_START; ++i) // npc 시야범위 내 있는 플레이어들에게 신호 보내는 곳
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

    if (n.m_LastAnim != n.m_anim)
        do_animation(npc_id, n.m_anim);

    if (ST_ACTIVE == g_clients[npc_id].m_status)
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

void Server::do_event_timer()
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
            case FUNC_NPC_FOLLOW:
            case FUNC_NPC_ATTACK:
            case FUNC_NPC_HOLD:
            case FUNC_DOT_DAMAGE:
            case FUNC_CHECK_FLAG:
            case FUNC_CHECK_TIME:
            case FUNC_CHECK_COLLISION:
            case FUNC_CHECK_GOLD:
            case FUNC_ARROW:
            {
                OverEx* over = new OverEx;
                over->function = (ENUM_FUNCTION)event.event_id;
                PostQueuedCompletionStatus(g_iocp, 1, event.obj_id, &over->over);
                break;
            }
            }
        }
    }
}

void Server::send_move_packet(int user_id, int other_id)
{
    sc_packet_move packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;

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

    //packet.move_time = g_clients[mover].m_move_time; // 스트레스 테스트
    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::send_gold_packet(int user_id)
{
    sc_packet_gold packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_GOLD;
    packet.gold = g_clients[user_id].m_gold;

    send_packet(user_id, &packet);
}

void Server::enter_game(int user_id, char name[])
{
    g_clients[user_id].m_cLock.lock(); // name, m_status 락
    strcpy_s(g_clients[user_id].m_name, name);
    g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // 마지막에 NULL 넣어주는 처리
    send_login_ok_packet(user_id); // 새로 접속한 플레이어 초기화 정보 보내줌
    g_clients[user_id].m_status = ST_ACTIVE; // 다른 클라들한테 정보 보낸 다음에 마지막에 ST_ACTIVE로 바꿔주기
    g_clients[user_id].m_cLock.unlock();
    for (int i = 0; i < 5; ++i)
        send_flag_info_packet(i, user_id); // 새로 접속한 플레이어 초기화 정보 보내줌
    cout << "Player " << user_id << " login finish" << endl;
    for (int i = 0; i <= MAX_USER; ++i)
    {
        if (user_id == i) // 데드락 회피용
            continue;

        if (true == is_near(user_id, i))
        {
            g_clients[i].m_cLock.lock();
            if (ST_SLEEP == g_clients[i].m_status)
            {
                g_clients[i].m_cLock.unlock();
                activate_npc(i, FUNC_NPC_HOLD);
            }
            else if (ST_ACTIVE == g_clients[i].m_status) // 이미 연결 중인 클라들한테만, m_status도 락을 걸어야 정상임
            {
                g_clients[i].m_cLock.unlock();
                send_enter_packet(user_id, i); // 새로 접속한 클라에게 이미 연결중인 클라 정보들을 보냄 
                if (true == is_player(i))
                    send_enter_packet(i, user_id); // 이미 접속한 플레이어들에게 새로 접속한 클라정보 보냄
            }
            else
                g_clients[i].m_cLock.unlock();
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

            if (0 < count)
            {
                //add_timer(-1, FUNC_CHECK_FLAG, 100);// 게임 플레이 시간 돌리는 함수
                add_timer(PURE_EVENT, FUNC_CHECK_TIME, 1000);// 게임 플레이 시간 돌리는 함수
                add_timer(PURE_EVENT, FUNC_CHECK_GOLD, 1000);// 게임 플레이 시간 돌리는 함수
                add_timer(PURE_EVENT, FUNC_CHECK_COLLISION, FRAME_TIME);// 게임 플레이 시간 돌리는 함수
                isGameStart = true;
                cout << "Game Routine Start!\n";
                break;
            }
        }
    }
}

void Server::initialize_objects()
{
    for (int i = 0; i < MAX_OBJECT; ++i)
    {
        g_clients[i].m_id = i; // 유저 등록
        g_clients[i].m_status = ST_FREE; // 여기는 멀티스레드 하기전에 싱글스레드일때 사용하는 함수, 락 불필요
        g_clients[i].m_type = TP_END;
    }

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

    ARROW_COUNT = 0;
}

void Server::initialize_NPC(int player_id)
{
    for (int npc_id = MY_NPC_START_SERVER(player_id); npc_id <= MY_NPC_END_SERVER(player_id); npc_id++)
    {
        if (ST_ACTIVE != g_clients[npc_id].m_status)
        {
            g_clients[player_id].m_gold -= 1;
            send_gold_packet(player_id);
            
            cout << npc_id << " is intit\n";
            g_clients[npc_id].m_socket = 0;
            g_clients[npc_id].m_id = npc_id;
            g_clients[npc_id].m_type = TP_NPC;
            g_clients[npc_id].m_owner_id = player_id;
            g_clients[npc_id].m_last_order = FUNC_NPC_FOLLOW;
            g_clients[npc_id].m_team = g_clients[player_id].m_team;
            sprintf_s(g_clients[npc_id].m_name, "NPC %d", npc_id);
            g_clients[npc_id].m_cLock.lock();
            g_clients[npc_id].m_status = ST_SLEEP;
            g_clients[npc_id].m_cLock.unlock();
            g_clients[npc_id].m_hp = SET_HP;
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_UP,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_UP));
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_LOOK,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK));
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_RIGHT,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_RIGHT));
            g_clients[player_id].m_transform.Scaling(SCALE.x, SCALE.y, SCALE.z);
            g_clients[npc_id].m_class = C_WORKER;
            update_speed_and_collider(npc_id);
            Ready_Collider_AABB_BOX(npc_id, g_clients[npc_id].m_col.aabb_size);
            Ready_Collider_OBB_BOX(npc_id, g_clients[npc_id].m_col.obb_size);
            g_clients[npc_id].m_rotate_speed = XMConvertToRadians(90.f);
            g_clients[npc_id].m_transform.SetUp_Speed(g_clients[npc_id].m_move_speed, g_clients[npc_id].m_rotate_speed);
            g_clients[npc_id].m_troop = T_INFT;

            g_clients[npc_id].m_isOut = false;
            g_clients[npc_id].m_isFormSet = true;
            g_clients[npc_id].m_attack_target = -1;
            g_clients[npc_id].m_isAttack = false;
            g_clients[npc_id].m_isOBB = false;

            FormationInfo formTemp;
            formTemp.id = npc_id, formTemp.final_pos = {}, formTemp.angle = 0.f, formTemp.radius = 0.f;
            g_clients[player_id].m_boid.push_back(formTemp);
            set_formation(player_id);
            for (int j = 0; j < g_clients[player_id].m_boid.size(); ++j)
            {
                if (g_clients[player_id].m_boid[j].id == g_clients[npc_id].m_id)
                {
                    g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION,
                        &g_clients[player_id].m_boid[j].final_pos);
                    g_clients[npc_id].m_boid_num = j;
                    g_clients[npc_id].m_total_angle = g_clients[player_id].m_boid[j].angle;
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
            send_npc_size_packet(player_id);
            break;
        }
        else
        {
            continue; // 여기 수정할것 (임시방편), 플레이어의 모든 npc active일때 더이상 추가 안되게 처리
        }
    }
}

void Server::do_change_npc_act(int player_id, unsigned char act)
{
    for (int npc_id = MY_NPC_START_SERVER(player_id); npc_id <= MY_NPC_END_SERVER(player_id); npc_id++)
    {
        if (ST_ACTIVE != g_clients[npc_id].m_status)
            continue;
        if (g_clients[player_id].m_troop == g_clients[npc_id].m_troop || g_clients[player_id].m_troop == T_ALL)
        {
            if (DO_FOLLOW == act)
            {
                g_clients[npc_id].m_last_order = FUNC_NPC_FOLLOW;
            }
            if (DO_ATTACK == act)
            {
                g_clients[npc_id].m_last_order = FUNC_NPC_ATTACK;
                g_clients[npc_id].m_target_look = *g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);

            }
            if (DO_HOLD == act)
            {
                g_clients[npc_id].m_last_order = FUNC_NPC_HOLD;
            }
            activate_npc(npc_id, g_clients[npc_id].m_last_order);
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
    packet.p_class = g_clients[other_id].m_class;
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

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_hp_packet(int user_id, int other_id)
{
    sc_packet_hp packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_HP;
    packet.id = other_id;
    packet.hp = g_clients[other_id].m_hp;

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_arrow_packet(int arrow_id, int user_id, int other_id)
{
    sc_packet_arrow packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ARROW;
    packet.shoot_id = other_id;
    packet.arrow_id = arrow_id;

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_deffend_packet(int deffend_id, int user_id, int other_id)
{
    sc_packet_deffend packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_DEFFEND;
    packet.deffend_id = deffend_id;
    _matrix mat = g_clients[deffend_id].m_transform.Get_Matrix();
    packet.r_x = mat._11;
    packet.r_y = mat._12;
    packet.r_z = mat._13;
    packet.u_x = mat._21;
    packet.u_y = mat._22;
    packet.u_z = mat._23;
    packet.l_x = mat._31;
    packet.l_y = mat._32;
    packet.l_z = mat._33;
    packet.p_x = mat._41;
    packet.p_z = mat._43;

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_do_particle_packet(int user_id, int other_id)
{
    sc_packet_do_particle packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_DO_PARTICLE;
    packet.id = other_id;

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_npc_size_packet(int user_id)
{
    sc_packet_npc_size packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_NPC_SIZE;
    packet.npc_size = g_clients[user_id].m_boid.size();
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_class_change_packet(int user_id, int other_id)
{
    sc_packet_class_change packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_CLASS_CHANGE;
    packet.id = other_id;
    packet.p_class = g_clients[other_id].m_class;
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_dead_packet(int user_id, int other_id)
{
    sc_packet_dead packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_DEAD;
    packet.id = g_clients[other_id].m_id;
    packet.hp = g_clients[other_id].m_hp;
    packet.anim = A_DEAD;
    //cout << user_id << " saw " << other_id << " dead\n";
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_invisible_packet(int user_id, int other_id, bool invi)
{
    sc_packet_invisible packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_INVISIBLE;
    packet.id = other_id;
    packet.invisable = invi;
    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_fire_packet(int id, float mx, float mz)
{
    sc_packet_fire packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_FIRE;
    packet.x = mx;
    packet.z = mz;
    send_packet(id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_teleport_packet(int id, float mx, float mz, unsigned char m)
{
    sc_packet_teleport packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_TELEPORT;
    packet.mode = m;
    packet.x = mx;
    packet.z = mz;
    send_packet(id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_leave_packet(int user_id, int other_id)
{
    sc_packet_leave packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_LEAVE;

    if (is_player(user_id))
    {
        g_clients[user_id].m_cLock.lock();
        g_clients[user_id].m_view_list.erase(other_id);
        g_clients[user_id].m_cLock.unlock();
    }

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::do_animation(int user_id, unsigned char anim)
{
    g_clients[user_id].m_LastAnim = anim;
    //cout << user_id << "is " << g_clients[user_id].m_anim << endl;
    if (user_id < NPC_START) // 애니메이션 신호를 보내온 애가 플레이어면
    {
        g_clients[user_id].m_cLock.lock();
        unordered_set<int> copy_viewlist = g_clients[user_id].m_view_list;
        g_clients[user_id].m_cLock.unlock();
        send_animation_packet(user_id, user_id, anim); // 임시
        for (auto cpy_vl : copy_viewlist) // 움직인 이후의 시야 범위에 대하여
        {
            if (is_player(cpy_vl)) // 다른 플레이어 에게만
                send_animation_packet(cpy_vl, user_id, anim); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
        }
    }
    else if (user_id >= NPC_START && user_id <= MAX_NPC) // 
    {
        for (int i = 0; i < NPC_START; ++i) // 모든 플레이어에 대해서
        {
            if (false == is_near(i, user_id)) // 근처에 없는 유저면 보내지도 마라
                continue;
            if (ST_ACTIVE != g_clients[i].m_status) // 로그인 상태 아닌애면 보내지 마라
                continue;

            send_animation_packet(i, user_id, anim); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
        }
    }
}

void Server::send_animation_packet(int user_id, int idler, unsigned char anim)
{
    sc_packet_animation packet;
    packet.id = idler;
    packet.size = sizeof(packet);
    packet.anim = anim;
    packet.type = SC_PACKET_ANIMATION;
    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::do_attack(int npc_id)
{
    SESSION& n = g_clients[npc_id];
    _vec3 n_look = *n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    n_look = -1.f * Vector3_::Normalize(n_look);
    _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    if (!n.m_isOBB) // OBB 충돌당한 상태가 아닐때
    {
        if (n.m_attack_target < 0) // 공격할 대상이 지정되지 않았을때
        {
            for (int i = 0; i < OBJECT_START; ++i) // 현재 주변에 적이 있나 확인
            {
                if (ST_ACTIVE != g_clients[i].m_status)
                    continue;
                if (ST_DEAD == g_clients[i].m_status)
                    continue;
                if (g_clients[i].m_team == n.m_team)
                    continue;
                if (!is_attack_detect(i, n.m_id))
                    continue;

                n.m_attack_target = i;
                break;
            }

            if (n.m_attack_target >= 0)
                return;
            else // 주변에 적이 없으면 탐색 움직임 시작
            {
                _vec3 t_look = g_clients[npc_id].m_target_look;
                t_look = -1.f * Vector3_::Normalize(t_look);

                float PdotProduct = (n_look.x * t_look.x) + (n_look.y * t_look.y) + (n_look.z * t_look.z); // 내각
                float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

                float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // 앞에 x 벡터 기준 각도 차이
                if (PoutProduct > 0) // 양수이면 n_look는 t_look로 부터 반시계
                    radian *= -1.f;

                float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지

                if (NPCangle > 3.f || NPCangle < -3.f) // npc가 바라보는 방향이 플레이어랑 일치하지 않을때
                {
                    n.m_transform.BackWard(TIME_DELTA * 2.f);
                    n.m_anim = A_WALK;
                    if (NPCangle > 3.f)
                    {
                        n.m_transform.Rotation_Y(-TIME_DELTA * 2.f);
                    }
                    else if (NPCangle < -3.f)
                    {
                        n.m_transform.Rotation_Y(TIME_DELTA * 2.f);
                    }
                }
                else // npc가 바라보는 방향이 플레이어랑 일치할때
                {
                    n.m_anim = A_WALK;
                    n.m_transform.BackWard(TIME_DELTA * 2.f);
                }
            }
        }
        else // 공격할 대상이 지정이 되었을때
        {
            if (ST_DEAD == g_clients[n.m_attack_target].m_status)
            {
                n.m_attack_target = -1;
                return;
            }
            _vec3* t_pos = g_clients[n.m_attack_target].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3* n_pos = n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3 t_look = *t_pos - *n_pos;
            t_look = Vector3_::Normalize(t_look);

            float PdotProduct = (n_look.x * t_look.x) + (n_look.y * t_look.y) + (n_look.z * t_look.z); // 내각
            float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

            float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // 앞에 x 벡터 기준 각도 차이
            if (PoutProduct > 0) // 양수이면 n_look는 t_look로 부터 반시계
                radian *= -1.f;

            float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지

            if (NPCangle > 3.f || NPCangle < -3.f) // npc가 공격할 대상을 안바라볼때
            {
                n.m_anim = A_WALK;
                if (NPCangle > 3.f)
                {
                    n.m_transform.Rotation_Y(-TIME_DELTA * 2.f);
                }
                else if (NPCangle < -3.f)
                {
                    n.m_transform.Rotation_Y(TIME_DELTA * 2.f);
                }
            }
            else // npc가 공격할 대상을 바라볼때
            {
                if (dist_between(n.m_id, n.m_attack_target) <= OBB_DIST &&
                    check_obb_collision(npc_id, n.m_attack_target))
                {
                    n.m_isAttack = true;
                    if (n.m_isAttack)
                    {
                        g_clients[n.m_attack_target].m_isOBB = true;
                        g_clients[n.m_attack_target].m_matAttackedTarget = n.m_transform.Get_Matrix();
                        n.m_isAttack = false;
                        g_clients[n.m_attack_target].m_hp -= ATTACK_DAMAGE;
                        if (g_clients[n.m_attack_target].m_hp > 0) // 상대방이 살아있을때
                        {
                            for (int i = 0; i < NPC_START; ++i)
                            {
                                if (ST_ACTIVE != g_clients[i].m_status)
                                    continue;
                                if (!is_near(i, n.m_attack_target))
                                    continue;
                                send_do_particle_packet(i, n.m_attack_target); // 남은 체력 브로드캐스팅
                                send_hp_packet(i, n.m_attack_target); // 남은 체력 브로드캐스팅
                            }
                        }
                        else // 상대방이 죽었을때
                        {
                            do_dead(n.m_attack_target);
                            n.m_attack_target = -1;
                        }
                    }
                    n.m_anim = A_ATTACK;
                }
                else // OBB 공격범위 밖일때
                {
                    n.m_transform.BackWard(TIME_DELTA * 2.f);
                }
            }
        }
    }
}

void Server::do_dead(int id)
{
    g_clients[id].m_cLock.lock();
    g_clients[id].m_status = ST_DEAD;
    g_clients[id].m_cLock.unlock();
    cout << id << " is dead\n";
    g_clients[id].m_hp = 0;
    for (int i = 0; i < NPC_START; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status && ST_DEAD != g_clients[i].m_status)
            continue;
        if (!is_near(i, id))
            continue;
        send_do_particle_packet(i, id); // 남은 체력 브로드캐스팅
        send_hp_packet(i, id); // 남은 체력 브로드캐스팅
        send_dead_packet(i, id); // 깎이고 남은 체력 있으면
        send_animation_packet(i, id, A_DEAD);
    }
    if (is_player(id))
    {
        // 죽는 애니메이션 끝나면 플레이어는 클라 애니메이션 끝났을때 부활신호 보내서 부활
    }
    else
    {
        // 죽는 애니메이션 끝나면 npc는 send_leave_packet(i, id); // 남은 체력 브로드캐스팅
    }
}

void Server::disconnect(int user_id)
{
    //send_leave_packet(user_id, user_id); // 나 자신
    cout << user_id << "is disconnect\n";
    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_status = ST_ALLOC; // 여기서 free 해버리면 아랫과정 진행중에 다른 클라에 할당될수도 있음
    closesocket(g_clients[user_id].m_socket);

    for (int i = 0; i < NPC_START; ++i)
    {
        if (user_id == i)
            continue;
        if (ST_ACTIVE == g_clients[i].m_status)
        {
            send_leave_packet(i, user_id); // 어차피 send_leave_packet 내부에서 뷰리스트 삭제 해줘서 여기에 따로 할 필요X
            for (int npc_id = MY_NPC_START_SERVER(user_id); npc_id <= MY_NPC_END_SERVER(user_id); ++npc_id)
            {
                if (ST_ACTIVE == g_clients[npc_id].m_status || ST_DEAD == g_clients[npc_id].m_status)
                    send_leave_packet(i, npc_id);
            }
        }
    }

    for (int i = MY_NPC_START_SERVER(user_id); i <= MY_NPC_END_SERVER(user_id); i++)
    {
        if (ST_ACTIVE == g_clients[i].m_status || ST_DEAD == g_clients[i].m_status)
        {
            g_clients[i].m_cLock.lock();
            g_clients[i].m_status = ST_SLEEP;
            g_clients[i].m_cLock.unlock();
        }
    }

    g_clients[user_id].m_boid.clear();
    g_clients[user_id].m_boid.shrink_to_fit();
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
        (a_pos->z - b_pos->z)) <= ATTACK_RADIUS)
        return true;
    else
        return false;
}

bool Server::is_attack_view(int attack, int gethit)
{
    _vec3 at_pos = *g_clients[attack].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3 at_look = *g_clients[attack].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    _vec3 gh_pos = *g_clients[gethit].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    _vec3 t_look = gh_pos - at_pos;
    t_look = Vector3_::Normalize(t_look);
    at_look = -1.f * Vector3_::Normalize(at_look);

    float PdotProduct = (at_look.x * t_look.x) + (at_look.y * t_look.y) + (at_look.z * t_look.z); // 내각
    float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

    float PoutProduct = (at_look.x * t_look.z) - (at_look.z * t_look.x); // 앞에 x 벡터 기준 각도 차이
    if (PoutProduct > 0) // 양수이면 t_look는 at_look로 부터 반시계
        radian *= -1.f;

    float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지

    if (NPCangle > 45.f || NPCangle < -45.f) // npc가 공격할 대상을 안바라볼때
        return false;
    else
        return true;
}

bool Server::is_attack_detect(int a, int b)
{
    _vec3* a_pos = g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* b_pos = g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    float aa = sqrt((a_pos->x - b_pos->x) *
        (a_pos->x - b_pos->x) +
        (a_pos->y - b_pos->y) *
        (a_pos->y - b_pos->y) +
        (a_pos->z - b_pos->z) *
        (a_pos->z - b_pos->z));

    if (sqrt((a_pos->x - b_pos->x) *
        (a_pos->x - b_pos->x) +
        (a_pos->y - b_pos->y) *
        (a_pos->y - b_pos->y) +
        (a_pos->z - b_pos->z) *
        (a_pos->z - b_pos->z)) < DETECT_RADIUS)
        return true;
    else
        return false;
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
                    //cout << "flag " << flag << " is red\n";
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
                    //cout << "flag " << flag << " is blue\n";
                    flags[flag].isBlue = true;
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
            //cout << "flag " << flag << " is not red\n";
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
            //cout << "flag " << flag << " is not blue\n";
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

void Server::set_starting_pos(int user_id)
{
    if (0 == user_id)
    {
        g_clients[user_id].m_team = TEAM_RED;
        _vec3 pos = { 350.f, 0.f, 400.f };
        g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
    }
    else
    {
        g_clients[user_id].m_team = TEAM_BLUE;
        _vec3 pos = { 340.f, 0.f, 420.f };
        g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
    }
    //{ // 스트레스 테스트
    //    _vec3 pos = { (float)(rand()%WORLD_HORIZONTAL), 0.f, (float)(rand() % WORLD_HORIZONTAL) };
    //    g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
    //}

    //flags[0].pos = { 50.f, 0.2f, 50.f };
    //flags[1].pos = { 100.f, 0.2f, 450.f };
    //flags[2].pos = { 250.f, 0.2f, 250.f };
    //flags[3].pos = { 450.f, 0.2f, 400.f };
    //flags[4].pos = { 450.f, 0.2f, 100.f };
    //곱하기 7.5배 / 맵사이즈  3750 - 3750
}

void Server::do_dot_damage(int id)
{
    SESSION& f = g_clients[id];
    for (int i = 0; i < OBJECT_START; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status)
            continue;
        if (g_clients[i].m_team == f.m_team)
            continue;
        if (dist_between(id, i) > FLAME_RANGE)
            continue;

        if (g_clients[i].m_hp > 0)
        {
            g_clients[i].m_hp -= DOT_DAMAGE;
            for (int j = 0; j < NPC_START; ++j)
            {
                if (ST_ACTIVE != g_clients[j].m_status)
                    continue;
                if (!is_near(i, j))
                    continue;
                send_hp_packet(j, i); // 남은 체력 브로드캐스팅
            }
        }
        else
        {
            if (ST_DEAD == g_clients[i].m_status)
                return;
            g_clients[i].m_hp = 0;
            //lock_guard <mutex> guardLock{ g_clients[att.m_attack_target].m_cLock };
            g_clients[i].m_status = ST_DEAD;
            cout << i << " is dead\n";
            for (int j = 0; j < NPC_START; ++j)
            {
                if (ST_ACTIVE != g_clients[j].m_status)
                    continue;
                if (!is_near(j, i))
                    continue;
                // 활성화 되어있고 맞은애 시야범위 안에 있는 유저일때
                send_leave_packet(j, i); // 남은 체력 브로드캐스팅
            }
        }
    }

    f.m_count++;

    if (f.m_count < 10)
        add_timer(id, FUNC_DOT_DAMAGE, 1000);
    else
    {
        g_clients[id].m_cLock.lock();
        g_clients[id].m_status = ST_SLEEP;
        g_clients[id].m_cLock.unlock();
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
            for (int i = 0; i <= MAX_USER; ++i)
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


                g_clients[user_id].m_transform.Rotation_Y(180 * (XM_PI / 180.0f));
                g_clients[user_id].m_transform.Scaling(SCALE.x, SCALE.y, SCALE.z);
                g_clients[user_id].m_class = C_WORKER;
                update_speed_and_collider(user_id);
                Ready_Collider_AABB_BOX(user_id, g_clients[user_id].m_col.aabb_size);
                Ready_Collider_OBB_BOX(user_id, g_clients[user_id].m_col.obb_size);
                g_clients[user_id].m_rotate_speed = XMConvertToRadians(90.f);
                g_clients[user_id].m_transform.SetUp_Speed(g_clients[user_id].m_move_speed, g_clients[user_id].m_rotate_speed);
                g_clients[user_id].m_last_order = FUNC_END;
                g_clients[user_id].m_formation = F_SQUARE;

                g_clients[user_id].m_owner_id = user_id;
                g_clients[user_id].m_type = TP_PLAYER;
                g_clients[user_id].m_hp = SET_HP;
                g_clients[user_id].m_total_angle = -90.f; // 수정
                g_clients[user_id].m_LastAnim = A_IDLE;
                g_clients[user_id].m_anim = A_IDLE;
                g_clients[user_id].m_troop = T_ALL;
                g_clients[user_id].m_owner_id = user_id; // 유저 등록
                g_clients[user_id].m_view_list.clear(); // 이전 뷰리스트 가지고 있으면 안되니 초기화
                g_clients[user_id].m_isAttack = false;
                g_clients[user_id].m_isOBB = false;
                set_starting_pos(user_id);

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
        case FUNC_NPC_ATTACK:
            finite_state_machine(id, FUNC_NPC_ATTACK);
            delete overEx;
            break;
        case FUNC_NPC_HOLD:
            finite_state_machine(id, FUNC_NPC_HOLD);
            delete overEx;
            break;
        case FUNC_NPC_FOLLOW:
            finite_state_machine(id, FUNC_NPC_FOLLOW);
            delete overEx;
            break;
        case FUNC_DOT_DAMAGE:
            do_dot_damage(id);
            delete overEx;
            break;
        case FUNC_ARROW:
            do_arrow(id);
            delete overEx;
            break;
       /* case FUNC_DEAD:
            cout << id << "is dead\n";
            if (is_player(id))
            {
                do_move(id, GO_COLLIDE);
            }
            else
            {
                g_clients[id].m_status = ST_SLEEP;
            }
            delete overEx;
            break;*/
        case FUNC_CHECK_FLAG:
            for (int i = 0; i < 5; ++i)
                is_flag_near(i);
            add_timer(-1, FUNC_CHECK_FLAG, 100);
            delete overEx;
            break;
        case FUNC_CHECK_TIME:
            if (play_time < 600)
            {
                play_time += 1;
                send_time_packet();
                add_timer(PURE_EVENT, FUNC_CHECK_TIME, 1000);
            }
            delete overEx;
            break;
        case FUNC_CHECK_COLLISION:
            if (isGameStart)
            {
                for (auto& it : g_clients) // OBB 작동한놈 OBB시키기
                {
                    if (ST_ACTIVE != it.second.m_status && ST_DEAD != it.second.m_status) // 활성화 된놈, 죽어있는놈은 제외
                        continue;
                    if(!it.second.m_isOBB)
                        continue;
                    Obb_Collision(it.first);
                }
                add_timer(PURE_EVENT, FUNC_CHECK_COLLISION, FRAME_TIME);
            }
            delete overEx;
            break;
        case FUNC_CHECK_GOLD:
            if (isGameStart)
            {
                for (int p_i = 0; p_i < NPC_START; ++p_i)
                {
                    if (ST_ACTIVE != g_clients[p_i].m_status && ST_DEAD != g_clients[p_i].m_status)
                        continue;
                    if (g_clients[p_i].m_gold >= 9)
                        continue;

                    g_clients[p_i].m_gold += 1;
                    send_gold_packet(p_i);
                }
                add_timer(PURE_EVENT, FUNC_CHECK_GOLD, 1500);
            }
            delete overEx;
            break;
        default:
            cout << "Unknown Operation in Worker_Thread\n";
            while (true);
        }
    }
}

void Server::check_aabb_collision(int o_mv,int o_ht)
{
    if (!is_object(o_ht))
    {
        if (dist_between(o_ht, o_mv) <= AABB_PN) // 4.f
            do_aabb(o_mv, o_ht);
    }
    else
    {
        if (TP_NATURE == g_clients[o_ht].m_type)
        {
            if (dist_between(o_ht, o_mv) <= AABB_NAT) // 50.f 
                do_aabb(o_mv, o_ht);
        }
        else if (TP_DEFFEND == g_clients[o_ht].m_type)
        {
            if (dist_between(o_ht, o_mv) <= AABB_DEF) // 10.f 
                do_aabb(o_mv, o_ht);
        }
    }
}

void Server::mainServer()
{
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
    initialize_objects(); // 오브젝트 정보들 초기화
    ready_timer();
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
    for (int i = 0; i < 4; ++i) // 여기에 쿼드코어라서 4 넣었는데 본인 코어수만큼 넣어도 ㄱㅊ
    {
        worker_threads.emplace_back([this]() {this->worker_thread(); });
    }

    //thread AI_thread([this]() {this->do_AI(); });
    //AI_thread.join();

    thread event_timer_thread([this]() {this->do_event_timer(); });

   /* float total_check = 0.f;
    high_resolution_clock::time_point check = high_resolution_clock::now();*/
    while (true)
    {
       /* duration<double> sec = high_resolution_clock::now() - check;
        if (sec >= seconds(1))
        {
            cout << total_check << endl;
            check = high_resolution_clock::now();
            total_check = 0.f;
        }*/
        this_thread::sleep_for(1ms);
        TIME_DELTA = time_delta();
        //for (int player_id = 0; player_id < NPC_START; ++player_id)
        //{
        //    if (ST_ACTIVE != g_clients[player_id].m_status && ST_DEAD != g_clients[player_id].m_status)
        //        continue;

        //    //send_time_delta(player_id, TIME_DELTA);
        //    for (int other_id = 0; other_id < NPC_START; ++other_id)
        //    {
        //        if (ST_ACTIVE != g_clients[other_id].m_status && ST_DEAD != g_clients[other_id].m_status)
        //            continue;
        //        if (!is_near(player_id, other_id))
        //            continue;
        //        send_move_packet(player_id, other_id);
        //    }
        //}
       
        //total_check += TIME_DELTA;
    }

    for (auto& t : worker_threads)
    {
        t.join();
    }
    closesocket(listenSocket);
    WSACleanup();
}

_matrix Server::Compute_WorldTransform(int id)
{
    _matrix matTransform = g_clients[id].m_transform.Get_Matrix();
    matTransform = Remove_Rotation(matTransform);

    return _matrix(matTransform);
}

_matrix Server::Remove_Rotation(_matrix matWorld)
{
    _vec3 vRight = _vec3(1.f, 0.f, 0.f);
    _vec3 vUp = _vec3(0.f, 1.f, 0.f);
    _vec3 vLook = _vec3(0.f, 0.f, 1.f);

    _vec3 vRightTemp = _vec3(matWorld.m[0][0], matWorld.m[0][1], matWorld.m[0][2]);
    _vec3 vUpTemp = _vec3(matWorld.m[1][0], matWorld.m[1][1], matWorld.m[1][2]);
    _vec3 vLookTemp = _vec3(matWorld.m[2][0], matWorld.m[2][1], matWorld.m[2][2]);

    vRight *= Vector3_::Length(vRightTemp);
    vUp *= Vector3_::Length(vUpTemp);
    vLook *= Vector3_::Length(vLookTemp);


    memcpy(&matWorld.m[0][0], &vRight, sizeof(_vec3));
    memcpy(&matWorld.m[1][0], &vUp, sizeof(_vec3));
    memcpy(&matWorld.m[2][0], &vLook, sizeof(_vec3));

    return _matrix(matWorld);
}

void Server::Update_Collider(int id, _vec3 vSize, COLLIDER_TYPE eType)
{
    _matrix pTarget_matrix = g_clients[id].m_transform.Get_Matrix();
    Collider& c = g_clients[id].m_col;
    switch (eType)
    {
    case COLLIDER_TYPE::COLLIDER_AABB:
    {
        _matrix matTemp = Remove_Rotation(pTarget_matrix);
        if (g_clients[id].m_class == C_MMAGE || g_clients[id].m_class == C_CAVALRY || g_clients[id].m_class == C_TWO)
        {
            matTemp.m[3][1] += 5.f;
        }
        else
        {
            matTemp.m[3][1] += 3.f;
        }
        matTemp.m[0][0] *= vSize.x;
        matTemp.m[1][1] *= vSize.y;
        matTemp.m[2][2] *= vSize.z;
        matTemp.m[3][1] += 3.f;

        c.m_ABvMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
        c.m_ABvMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };
    }
    break;
    case COLLIDER_TYPE::COLLIDER_OBB:
    {

        _matrix matTemp_Rotate = pTarget_matrix;
        _matrix matTemp;
        matTemp.m[0][0] *= vSize.x;
        matTemp.m[1][1] *= vSize.y;
        matTemp.m[2][2] *= vSize.z;

        matTemp = matTemp * matTemp_Rotate;
        if (g_clients[id].m_class == C_MMAGE || g_clients[id].m_class == C_CAVALRY || g_clients[id].m_class == C_TWO)
        {
            matTemp.m[3][1] += 5.f;
        }
        else
        {
            matTemp.m[3][1] += 3.f;
        }
       c.m_OBvMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
       c.m_OBvMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };

       c.m_pOBB.vPoint[0] = _vec3(c.m_OBvMin.x, c.m_OBvMax.y, c.m_OBvMin.z);
       c.m_pOBB.vPoint[1] = _vec3(c.m_OBvMax.x, c.m_OBvMax.y, c.m_OBvMin.z);
       c.m_pOBB.vPoint[2] = _vec3(c.m_OBvMax.x, c.m_OBvMin.y, c.m_OBvMin.z);
       c.m_pOBB.vPoint[3] = _vec3(c.m_OBvMin.x, c.m_OBvMin.y, c.m_OBvMin.z);
                                      
       c.m_pOBB.vPoint[4] = _vec3(c.m_OBvMin.x, c.m_OBvMax.y, c.m_OBvMax.z);
       c.m_pOBB.vPoint[5] = _vec3(c.m_OBvMax.x, c.m_OBvMax.y, c.m_OBvMax.z);
       c.m_pOBB.vPoint[6] = _vec3(c.m_OBvMax.x, c.m_OBvMin.y, c.m_OBvMax.z);
       c.m_pOBB.vPoint[7] = _vec3(c.m_OBvMin.x, c.m_OBvMin.y, c.m_OBvMax.z);
    }                                          
    break;
    default:
        break;
    }
}

void Server::Ready_Collider_AABB_BOX(int id, const _vec3 vSize)
{
    _matrix pTarget_matrix = g_clients[id].m_transform.Get_Matrix();
    _matrix matTemp = Remove_Rotation(pTarget_matrix);

    CTransform m_pTransformCom;
    m_pTransformCom.Set_Matrix(&matTemp);
    m_pTransformCom.Scaling(vSize);

    g_clients[id].m_col.m_ABvMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
    g_clients[id].m_col.m_ABvMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };
}

void Server::Ready_Collider_OBB_BOX(int id, const _vec3 vSize)
{
    CTransform* pTarget_Transform = &g_clients[id].m_transform;
    Collider& c = g_clients[id].m_col;
    _vec3		vDir[3];

    for (size_t i = 0; i < 3; ++i)
    {
        vDir[i] = *pTarget_Transform->Get_StateInfo(CTransform::STATE(i));
        vDir[i] = Vector3_::Normalize(vDir[i]);
    }

    c.m_OBvMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
    c.m_OBvMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };

    c.m_pOBB.vPoint[0] = _vec3(c.m_OBvMin.x, c.m_OBvMax.y, c.m_OBvMin.z);
    c.m_pOBB.vPoint[1] = _vec3(c.m_OBvMax.x, c.m_OBvMax.y, c.m_OBvMin.z);
    c.m_pOBB.vPoint[2] = _vec3(c.m_OBvMax.x, c.m_OBvMin.y, c.m_OBvMin.z);
    c.m_pOBB.vPoint[3] = _vec3(c.m_OBvMin.x, c.m_OBvMin.y, c.m_OBvMin.z);
                                   
    c.m_pOBB.vPoint[4] = _vec3(c.m_OBvMin.x, c.m_OBvMax.y, c.m_OBvMax.z);
    c.m_pOBB.vPoint[5] = _vec3(c.m_OBvMax.x, c.m_OBvMax.y, c.m_OBvMax.z);
    c.m_pOBB.vPoint[6] = _vec3(c.m_OBvMax.x, c.m_OBvMin.y, c.m_OBvMax.z);
    c.m_pOBB.vPoint[7] = _vec3(c.m_OBvMin.x, c.m_OBvMin.y, c.m_OBvMax.z);
}

void Server::do_aabb(int a, int b) // (CCollider* pTargetCollider, CTransform* pSourTransform, CTransform* pDestTransform)
{
    _matrix		matSour = Compute_WorldTransform(a);
    _matrix		matDest = Compute_WorldTransform(b);

    _vec3		vSourMin, vSourMax;
    _vec3		vDestMin, vDestMax;

    XMMATRIX	xmMatSour = XMLoadFloat4x4(&matSour);
    XMMATRIX	xmMatDest = XMLoadFloat4x4(&matDest);
    //XMLoadFloat4x4
    vSourMin = Vector3_::TransformCoord(g_clients[a].m_col.m_ABvMin, xmMatSour);
    vSourMax = Vector3_::TransformCoord(g_clients[a].m_col.m_ABvMax, xmMatSour);
                                                             
    vDestMin = Vector3_::TransformCoord(g_clients[b].m_col.m_ABvMin, xmMatDest);
    vDestMax = Vector3_::TransformCoord(g_clients[b].m_col.m_ABvMax, xmMatDest);

    _vec3 vSourPos = *g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3 vDestPos = *g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    _float	fMoveX = (min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
    _float	fMoveZ = (min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z));
    if (max(vSourMin.x, vDestMin.x) < min(vSourMax.x, vDestMax.x) &&
        max(vSourMin.z, vDestMin.z) < min(vSourMax.z, vDestMax.z))
    {
        if (abs(fMoveX) < abs(fMoveZ))
        {
            if (vSourPos.x < vDestPos.x)
            {
                _vec3	vTemp = { g_clients[b].m_transform.Get_Matrix()._41 + fMoveX,
                                  g_clients[b].m_transform.Get_Matrix()._42,
                                  g_clients[b].m_transform.Get_Matrix()._43 };
                g_clients[b].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
            }
            else
            {
                _vec3	vTemp = { g_clients[b].m_transform.Get_Matrix()._41 - fMoveX,
                                  g_clients[b].m_transform.Get_Matrix()._42,
                                  g_clients[b].m_transform.Get_Matrix()._43 };
                g_clients[b].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
            }
        }
        else
        {
            if (vSourPos.z < vDestPos.z)
            {
                _vec3	vTemp = { g_clients[b].m_transform.Get_Matrix()._41,
                                  g_clients[b].m_transform.Get_Matrix()._42,
                                  g_clients[b].m_transform.Get_Matrix()._43 + fMoveZ };
                g_clients[b].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
            }
            else
            {
                _vec3	vTemp = { g_clients[b].m_transform.Get_Matrix()._41 ,
                                  g_clients[b].m_transform.Get_Matrix()._42,
                                  g_clients[b].m_transform.Get_Matrix()._43 - fMoveZ };
                g_clients[b].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
            }
        }

        Update_Collider(b, g_clients[b].m_col.aabb_size, COLLIDER_TYPE::COLLIDER_AABB);
        Update_Collider(b, g_clients[b].m_col.obb_size, COLLIDER_TYPE::COLLIDER_OBB);

        for (int i = 0; i < NPC_START; ++i) // 밀림 당한애 좌표값 업데이트
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (!is_near(i, b))
                continue;
            send_move_packet(i, b);
        }
    }
}

void Server::do_arrow(int arrow_id)
{
    duration<double> arrow_life = duration_cast<duration<double>>(high_resolution_clock::now()
        - g_clients[arrow_id].m_lifetime);
    if (arrow_life.count() < ARROW_ENDTIME) // 아직 화살 유지시간 남아있을때
    {
        g_clients[arrow_id].m_transform.Go_Right(TIME_DELTA);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            send_move_packet(i, arrow_id); // 남은 체력 브로드캐스팅
        }
        do_arrow_collision(arrow_id);
        if (ST_ACTIVE == g_clients[arrow_id].m_status)
            add_timer(arrow_id, FUNC_ARROW, FRAME_TIME);
    }
    else // 화살 유지시간 끝났을때
        delete_arrow(arrow_id);
}

void Server::delete_arrow(int arrow_id)
{
    g_clients[arrow_id].m_cLock.lock();
    g_clients[arrow_id].m_status = ST_SLEEP;
    g_clients[arrow_id].m_cLock.unlock();
    g_clients[arrow_id].m_transform.Ready_Transform();
    for (int i = 0; i < NPC_START; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status)
            continue;
        send_leave_packet(i, arrow_id);
    }
    ARROW_COUNT--;
}

void Server::do_arrow_collision(int arrow_id)
{
    for (int arrow = OBJECT_START; arrow < MAX_OBJECT; ++arrow)
    {
        for (auto& iter : g_clients)
        {
            if (ST_ACTIVE != iter.second.m_status)
                continue;
            if (arrow == iter.first) // 자기 자신 제외
                continue;
            if (g_clients[arrow].m_owner_id == iter.first) // 화살 쏜 주인 제외
                continue;
            if (TP_ARROW == iter.second.m_type || TP_SKILL == iter.second.m_type) // 화살, 스킬이랑 충돌 제외
                continue;

            // 나머지 플레이어, npc, 자연물, deffend 상대로 충돌
            _float fLength = 0.f;
            _vec3 arrow_Pos = *g_clients[arrow].m_transform.Get_StateInfo(CTransform::STATE_POSITION); // Arrow
            _vec3 gothit_Pos = *iter.second.m_transform.Get_StateInfo(CTransform::STATE_POSITION); // other

            _vec3 vDistance = gothit_Pos - arrow_Pos;
            fLength = vDistance.Length();

            if (fLength <= ARROW_DIST)
            {
                if (iter.second.m_type == TP_NATURE || iter.second.m_type == TP_DEFFEND) // 화살 맞은애가 밀리면 안됨
                {
                    iter.second.m_hp -= ARROW_DAMAGE;
                    for (int i = 0; i < NPC_START; ++i)
                    {
                        if (ST_ACTIVE != g_clients[i].m_status)
                            continue;
                        if (!is_near(i, iter.first))
                            continue;
                        send_do_particle_packet(i, iter.first); // 남은 체력 브로드캐스팅
                        send_hp_packet(i, iter.first); // 남은 체력 브로드캐스팅
                    }
                    delete_arrow(arrow_id);
                }
                else // 플레이어,npc,동물은 맞으면 obb 밀려야함
                {
                    _vec3 arrow_Pos = *g_clients[arrow].m_transform.Get_StateInfo(CTransform::STATE_POSITION); // Arrow
                    iter.second.m_matAttackedTarget._41 = arrow_Pos.x;
                    iter.second.m_matAttackedTarget._42 = arrow_Pos.y;
                    iter.second.m_matAttackedTarget._43 = arrow_Pos.z;
                    iter.second.m_hp -= ARROW_DAMAGE;
                    iter.second.m_isOBB = true;
                    for (int i = 0; i < NPC_START; ++i)
                    {
                        if (ST_ACTIVE != g_clients[i].m_status)
                            continue;
                        if (!is_near(i, iter.first))
                            continue;
                        send_do_particle_packet(i, iter.first); // 남은 체력 브로드캐스팅
                        send_hp_packet(i, iter.first); // 남은 체력 브로드캐스팅
                    }
                    delete_arrow(arrow_id);
                }
            }
        }
    }
}

bool Server::check_obb_collision(int a, int b)
{
    OBB* pTargetOBB = &g_clients[b].m_col.m_pOBB;
	if (nullptr == pTargetOBB)
		return false;

	OBB			tOBB[2];
	ZeroMemory(tOBB, sizeof(OBB) * 2);

	for (size_t i = 0; i < 8; ++i)
	{
        _matrix		matWorld = Compute_WorldTransform(a);
        _matrix		matTargetWorld = Compute_WorldTransform(b);

		tOBB[0].vPoint[i] = _vec3::Transform(g_clients[a].m_col.m_pOBB.vPoint[i], matWorld);
		tOBB[1].vPoint[i] = _vec3::Transform(pTargetOBB->vPoint[i], matTargetWorld);

	}

	tOBB[0].vCenter = (tOBB[0].vPoint[0] + tOBB[0].vPoint[6]) * 0.5f;
	tOBB[1].vCenter = (tOBB[1].vPoint[0] + tOBB[1].vPoint[6]) * 0.5f;

	for (size_t i = 0; i < 2; ++i)
	{
		Compute_AlignAxis(&tOBB[i]);
		Compute_ProjAxis(&tOBB[i]);
	}

	_vec3	vAlignAxis[9];

	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			_uint		iIndex = i * 3 + j;
			vAlignAxis[iIndex] = Vector3_::CrossProduct(tOBB[0].vAlignAxis[i], tOBB[1].vAlignAxis[j], false);
		}
	}

	_float		fDistance[3] = {};
	for (size_t i = 0; i < 2; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			_vec3		vCenterDir = tOBB[1].vCenter - tOBB[0].vCenter;

			fDistance[0] = fabs(vCenterDir.Dot(tOBB[i].vAlignAxis[j]));


			fDistance[1] =
				fabs(tOBB[0].vProjAxis[0].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[0].vProjAxis[1].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[0].vProjAxis[2].Dot(tOBB[i].vAlignAxis[j]));


			fDistance[2] =
				fabs(tOBB[1].vProjAxis[0].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[1].vProjAxis[1].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[1].vProjAxis[2].Dot(tOBB[i].vAlignAxis[j]));


			if (fDistance[1] + fDistance[2] < fDistance[0])
				return true;

		}
	}

	for (size_t i = 0; i < 9; ++i)
	{
		_vec3		vCenterDir = tOBB[1].vCenter - tOBB[0].vCenter;
		fDistance[0] = fabs(vCenterDir.Dot(vAlignAxis[i]));

		fDistance[1] =
			fabs(tOBB[0].vProjAxis[0].Dot(vAlignAxis[i])) +
			fabs(tOBB[0].vProjAxis[1].Dot(vAlignAxis[i])) +
			fabs(tOBB[0].vProjAxis[2].Dot(vAlignAxis[i]));

		fDistance[2] =
			fabs(tOBB[1].vProjAxis[0].Dot(vAlignAxis[i])) +
			fabs(tOBB[1].vProjAxis[1].Dot(vAlignAxis[i])) +
			fabs(tOBB[1].vProjAxis[2].Dot(vAlignAxis[i]));

		if (fDistance[1] + fDistance[2] < fDistance[0])
			return false;
	}
	return true;
}

bool Server::is_object(int id)
{
    if (id >= OBJECT_START)
        return true;
    else
        return false;
}

void Server::Compute_AlignAxis(OBB* pOBB)
{
    pOBB->vAlignAxis[0] = (pOBB->vPoint[2] - pOBB->vPoint[3]);
    pOBB->vAlignAxis[1] = (pOBB->vPoint[0] - pOBB->vPoint[3]);
    pOBB->vAlignAxis[2] = (pOBB->vPoint[7] - pOBB->vPoint[3]);

    for (size_t i = 0; i < 3; ++i)
    {
        pOBB->vAlignAxis[i] = Vector3_::Normalize(pOBB->vAlignAxis[i]);
    }
}

void Server::Compute_ProjAxis(OBB* pOBB)
{
    _vec3 vAdd[3];
    vAdd[0] = Vector3_::Add(pOBB->vPoint[5], pOBB->vPoint[2]);
    vAdd[1] = Vector3_::Add(pOBB->vPoint[5], pOBB->vPoint[0]);
    vAdd[2] = Vector3_::Add(pOBB->vPoint[5], pOBB->vPoint[7]);

    _vec3 vProd[3];
    vProd[0] = Vector3_::ScalarProduct(vAdd[0], 0.5f, false);
    vProd[1] = Vector3_::ScalarProduct(vAdd[1], 0.5f, false);
    vProd[2] = Vector3_::ScalarProduct(vAdd[2], 0.5f, false);

    pOBB->vProjAxis[0] = (vProd[0] - pOBB->vCenter);
    pOBB->vProjAxis[1] = (vProd[1] - pOBB->vCenter);
    pOBB->vProjAxis[2] = (vProd[2] - pOBB->vCenter);
}

void Server::Obb_Collision(int id)
{
    SESSION& o = g_clients[id];
    if (o.m_isOBB && o.m_fBazierCnt <= 1.f)
    {
        if (!o.m_isBazier)
        {
            _vec3 vTargetPos = { o.m_matAttackedTarget.m[3][0], o.m_matAttackedTarget.m[3][1], o.m_matAttackedTarget.m[3][2] };
            _vec3 vPos = *o.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3 vTemp = { vPos - vTargetPos };
            vTemp *= 3.f;
            o.m_vStartPoint = vPos;
            o.m_vEndPoint = *o.m_transform.Get_StateInfo(CTransform::STATE_POSITION) + (vTemp);
            o.m_vMidPoint = (o.m_vStartPoint + o.m_vEndPoint) / 2;
            o.m_vMidPoint.y += 10.f;
            o.m_isBazier = true;
        }
        Hit_Object(id, o.m_fBazierCnt, o.m_vStartPoint, o.m_vEndPoint, o.m_vMidPoint);
    }
    if (o.m_fBazierCnt >= 1.f)
    {
        o.m_fBazierCnt = 0.f;
        o.m_isOBB = false;
        o.m_isBazier = false;
        o.m_attack_target = -1;
        _matrix temp = o.m_transform.Get_Matrix();
        temp._42 = 0.f;
        o.m_transform.Set_Matrix(&temp);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (!is_near(i, id))
                continue;
            send_move_packet(i, id);
        }
    }
}

void Server::Hit_Object(int id, _float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid)
{
    _float fX = (pow((1.f - fCnt), 2) * vStart.x) + (2 * fCnt * (1.f - fCnt) * vMid.x) + (pow(fCnt, 2) * vEnd.x);
    _float fY = (pow((1.f - fCnt), 2) * vStart.y) + (2 * fCnt * (1.f - fCnt) * vMid.y) + (pow(fCnt, 2) * vEnd.y);
    _float fZ = (pow((1.f - fCnt), 2) * vStart.z) + (2 * fCnt * (1.f - fCnt) * vMid.z) + (pow(fCnt, 2) * vEnd.z);

    _vec3 vPos = { fX, fY, fZ };
    g_clients[id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &vPos);
    for (int i = 0; i < NPC_START; ++i)
    {
        if (ST_ACTIVE != g_clients[i].m_status && ST_DEAD != g_clients[i].m_status)
            continue;
        if (!is_near(i, id))
            continue;
        send_move_packet(i, id);
    }
    fCnt += 0.02f;
}

void Server::cal_change_class_gold(int id, short m_class)
{
    if (C_WORKER == m_class)
    {
        g_clients[id].m_gold -= TIER_ONE_PRICE;
    }
    if (C_CAVALRY == m_class)
    {
        g_clients[id].m_gold -= TIER_ONE_PRICE;
    }
    if (C_TWO == m_class)
    {
        g_clients[id].m_gold -= TIER_TWO_PRICE;
    }
    if (C_INFANTRY == m_class)
    {
        g_clients[id].m_gold -= TIER_ONE_PRICE;
    }
    if (C_FOUR == m_class)
    {
        g_clients[id].m_gold -= TIER_TWO_PRICE;
    }
    if (C_SPEARMAN == m_class)
    {
        g_clients[id].m_gold -= TIER_ONE_PRICE;
    }
    if (C_MAGE == m_class)
    {
        g_clients[id].m_gold -= TIER_ONE_PRICE;
    }
    if (C_MMAGE == m_class)
    {
        g_clients[id].m_gold -= TIER_TWO_PRICE;
    }
    if (C_ARCHER == m_class)
    {
        g_clients[id].m_gold -= TIER_ONE_PRICE;
    }

    send_gold_packet(id);
}
