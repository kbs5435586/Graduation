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
    wcout << L"���� " << lpMsgBuf << endl;
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
    int rest_byte = io_byte; // �̸�ŭ ���Ҵ�, �̸�ŭ ó���� ���� ������Ѵ�

    char* p = g_clients[user_id].m_recv_over.io_buf;// ���� �߿��� ��� �κ��� ó���ϰ� �ִ�, ��Ŷ�� ó���Ҽ��� ó���� ��Ŷ ���� �����Ϳ� ������ �ؾ���
    //�츮�� ó���ؾ��� �����Ϳ� ���� ������, ó�� �����ϴ°Ŵϱ� io_buf�� ����ִ� ������ �� �պ��� �����ؾ���
    int packet_size = 0;

    if (0 != g_clients[user_id].m_prev_size) // ������ �޾Ƴ��� �����Ͱ� ������
        packet_size = g_clients[user_id].m_packet_buf[0];

    while (rest_byte > 0) // ó���ؾ��� �����Ͱ� ����������
    {
        if (0 == packet_size) // ���� �츮�� ó���ϴ� ��Ŷ�� ó���϶� -> �����͸� ��Ŷ ������ �� ���� ����Ű�� ����
            packet_size = *p;

        if (packet_size <= rest_byte + g_clients[user_id].m_prev_size) // �������� �����Ŷ� �������� ���ļ� ��Ŷ ������� ũ�ų� ������ ��Ŷ �ϼ�
        {
            memcpy(g_clients[user_id].m_packet_buf + g_clients[user_id].m_prev_size, p, packet_size - g_clients[user_id].m_prev_size); // p�� �ִ°� packet_size��ŭ m_packet_buf�� ����

            p += packet_size - g_clients[user_id].m_prev_size;
            rest_byte -= packet_size - g_clients[user_id].m_prev_size;
            packet_size = 0;
            process_packet(user_id, g_clients[user_id].m_packet_buf);
            g_clients[user_id].m_prev_size = 0;
        }
        else // ���ĵ� ��Ŷ ���̺꺸�� �۴�, ��Ŷ �ϼ��� ����Ų��, �������� �����س��� ��������
        {
            memcpy(g_clients[user_id].m_packet_buf + g_clients[user_id].m_prev_size, p, rest_byte); // p�� �ִ°� rest_byte��ŭ m_packet_buf�� ����
            // Ȥ�ö� 2���̻� �޾Ҵµ� ��Ŷ �ϼ� ����Ų ��찡 ���� �� ������ ������ �޾Ƴ��� ũ�� �ں��� �����ؿ��� ����
            g_clients[user_id].m_prev_size += rest_byte;
            rest_byte = 0;
            p += rest_byte; // ó�� �������� �׸�ŭ ������ ��ġ �̵�
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

        float PdotProduct = Vector3_::DotProduct(oldLook, newLook); // ����
        float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

        float PoutProduct = (oldLook.x * newLook.z) - (oldLook.z * newLook.x); // �տ� x ���� ���� ���� ����
        if (PoutProduct > 0) // ����̸� newLook�� oldLook�� ���� �ݽð�
            radian *= -1.f;

        float angle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
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

        float PdotProduct = Vector3_::DotProduct(oldLook, newLook); // ����
        float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

        float PoutProduct = (oldLook.x * newLook.z) - (oldLook.z * newLook.x); // �տ� x ���� ���� ���� ����
        if (PoutProduct > 0) // ����̸� newLook�� oldLook�� ���� �ݽð�
            radian *= -1.f;

        float angle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
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

    for (auto& o : g_clients) // aabb �浹üũ
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
        if (false == is_near(c.second.m_id, user_id)) // ��ó�� ���¾ִ� �׳� �������� ����
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (c.second.m_id == user_id)
            continue;
        new_viewlist.insert(c.second.m_id); // �� �þ� �����ȿ� ������ �ٸ� ��ü���� ���̵� ����
    }

    // send_move_packet ���ִ� �κ�
    send_move_packet(user_id, user_id); // ������ ���� �̵��Ѱ� �˷��ִ� �뵵

    for (auto new_vl : new_viewlist) // ������ ������ �þ� ������ ���Ͽ�
    {
        if (0 == old_viewlist.count(new_vl)) // ���� �丮��Ʈ�� new_vl�� ������ 0�� �϶� = ���� �þ߿� ���� ���϶�
        {
            send_enter_packet(user_id, new_vl); // �ٸ� ��ü���� ������ ������ ����

            if (false == is_player(new_vl)) // ���� �þ߿� ���� �ְ� �÷��̾ �ƴϸ� �� �ݺ��� �ѱ� / �� �ƴ϶� npc �ߵ����ְ� �ѱ�
            {
                continue;
            }

            g_clients[new_vl].m_cLock.lock();
            if (0 == g_clients[new_vl].m_view_list.count(user_id)) // ����� �丮��Ʈ�� ���� ���ٸ�
            {
                g_clients[new_vl].m_cLock.unlock();
                send_enter_packet(new_vl, user_id); // ���� ���� ������ �ٸ� ��ü�鿡�� ����
            }
            else
            {
                g_clients[new_vl].m_cLock.unlock();
                send_move_packet(new_vl, user_id); // ���� ������ ������ �ٸ� ��ü�鿡�� ����
            }
        }
        else // �̵� �� �Ŀ� �� �þ߿� ���̴� �÷��̾��ε� �������� ������ �ִ� = ���� �þ߿� �ִ� ��
        {
            if (false == is_player(new_vl)) // npc���� �� ���ο� �������� ������ �ʿ䰡 ����
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

    for (auto old_vl : old_viewlist) // �����̱� �� �þ߹����� ���Ͽ�
    {
        if (0 == new_viewlist.count(old_vl)) // �� �þ߹����� old_vl ������ 0�϶� = �þ� �������� ��� ��ü�϶�
        {
            send_leave_packet(user_id, old_vl); // ������ ��� ��ü�� ������ �˸�

            if (false == is_player(old_vl)) // npc���� ���� �����ٴ°� �Ⱦ˷��� �ȴ�
                continue;

            g_clients[old_vl].m_cLock.lock();
            if (0 != g_clients[old_vl].m_view_list.count(user_id))
            {
                g_clients[old_vl].m_cLock.unlock();
                send_leave_packet(old_vl, user_id); // ��� ��ü���� ���� ������ �˸�
            }
            else // �Ǽ��ϱ� ����, else�� �� ������� unlock ���ٰ�, �ȱ׷��� ���� �Ҹ����� �� ��Ǯ��
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
                        send_do_particle_packet(player, i); // ���� ü�� ��ε�ĳ����
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

        for (int i = 0; i < NPC_START; ++i) // npc �þ߹��� �� �ִ� �÷��̾�鿡�� ��ȣ ������ ��
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


    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
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

    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::send_flag_bool_packet(int object_id, int user_id)
{
    sc_packet_flag_bool packet;
    packet.id = object_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_FLAG_BOOL;
    packet.isBlue = flags[object_id].isBlue;
    packet.isRed = flags[object_id].isRed;

    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
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

    // m_recv_over�� recv ���� ������ ����ü�̹Ƿ� ���� �ȵȴ�
    // �׳� OverEx overex�� ������ ����ü �����ؼ� ���°͵� �ȵȴ�, ���� ������ �Լ� ������ ������ ��������� -> �Ҵ�޾ƾ���
    OverEx* overEx = new OverEx; // Ȯ�� ������ �Ҵ�
    overEx->function = FUNC_SEND; // Ÿ�� ����
    ZeroMemory(&overEx->over, sizeof(overEx->over)); // ������ ����ü �ʱ�ȭ
    memcpy(overEx->io_buf, buf, buf[0]); // IOCP���ۿ� ��Ŷ ������ ��Ŷ ũ�⸸ŭ ����
    overEx->wsabuf.buf = overEx->io_buf; // WSA���ۿ� IOCP���� ����
    overEx->wsabuf.len = buf[0]; // ���� ������ ����

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
            if (n_pos != g_clients[n.m_owner_id].m_boid[i].final_pos) // �ڽ��� ������ ��ġ�� �ƴҶ�
            {
                n.m_anim = A_IDLE;
                if (dist_between(npc_id, n.m_owner_id) > g_clients[n.m_owner_id].m_boid[i].radius + NEAR_APPROACH
                    || dist_between(npc_id, n.m_owner_id) < g_clients[n.m_owner_id].m_boid[i].radius - NEAR_APPROACH) // ������ �ݰ� ���̳� ���϶�
                {
                    n.m_anim = A_WALK;
                    _vec3 Dir = move_to_spot(npc_id, &g_clients[n.m_owner_id].m_boid[i].final_pos);
                    _vec3 new_pos = n_pos + Dir;
                    n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &new_pos);

                    _vec3 t_pos = g_clients[n.m_owner_id].m_boid[i].final_pos; // �ڱⰡ ������ ��ǥ��ġ�� �ȹٶ󺼶�
                    _vec3 t_look = Vector3_::Subtract(t_pos, n_pos);
                    t_look = Vector3_::Normalize(t_look);

                    float PdotProduct = Vector3_::DotProduct(n_look, t_look);
                    float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

                    float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // �տ� x ���� ���� ���� ����
                    if (PoutProduct > 0) // ����̸� n_look�� t_look�� ���� �ݽð�
                        radian *= -1.f;

                    float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� ��ǥ��ġ ���� � ���̳�����

                    if (NPCangle > 2.0f || NPCangle < -2.0f) // npc�� ��ǥ��ġ�� �ȹٶ󺼶�
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
                    if (n.m_isOut) // ���� �� ó�� ������ �ݰ濡 ��������
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
                    else // �̹� ������ �ݰ濡 �ִ� ���϶�
                    {
                        if (g_clients[n.m_owner_id].m_boid[i].angle + 3.f < n.m_total_angle // ������ ȸ�������� �ƴҶ�
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
                        else // ������ ȸ������ �϶�
                        {
                            _vec3 Dir = move_to_spot(npc_id, &g_clients[n.m_owner_id].m_boid[i].final_pos);
                            _vec3 new_pos = n_pos + Dir;
                            n.m_transform.Set_StateInfo(CTransform::STATE_POSITION, &new_pos);
                        }

                        n_look = Vector3_::Normalize(n_look);
                        p_look = Vector3_::Normalize(p_look);
                        float PdotProduct = Vector3_::DotProduct(n_look, p_look);
                        float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

                        float PoutProduct = (p_look.x * n_look.z) - (p_look.z * n_look.x); // �տ� x ���� ���� ���� ����
                        if (PoutProduct > 0) // ����̸� n_look�� t_look�� ���� �ݽð�
                            radian *= -1.f;

                        float NPCangle = radian * 180.f / PIE; // ���� npc look�� player look ���� � ���̳�����

                        if (NPCangle > 2.f || NPCangle < -2.f) // npc�� look�� �ȹٶ󺼶�
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
            else // �ڽ��� ������ ��ġ�϶�
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
            Dir = Dir * g_clients[id].m_move_speed * TIME_DELTA * 2.f; // ��ְ� �������� ��ŭ ���� ���
        }
    }
    return Dir;
}

void Server::activate_npc(int npc_id, ENUM_FUNCTION op_type)
{
    ENUM_STATUS old_status = ST_SLEEP;
    if (true == atomic_compare_exchange_strong(&g_clients[npc_id].m_status, &old_status, ST_ACTIVE)) // m_status�� �������� ��Ƽ��� �ٲ� ��쿡��
        // ���ÿ� �� Ŭ�� �����ϸ� ACTIVE �� 2�� �ٲ�� Ÿ�̸Ӱ� 2�� �ߵ��ϴ°� �����ϱ� ���� �뵵
        add_timer(npc_id, op_type, FRAME_TIME);
}

void Server::finite_state_machine(int npc_id, ENUM_FUNCTION func_id)
{
    SESSION& n = g_clients[npc_id];

    if (ST_ACTIVE == g_clients[n.m_owner_id].m_status && ST_ACTIVE == g_clients[npc_id].m_status) // NPC�� ������ �÷��̾ Ȱ��ȭ �Ǿ� ������
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

    for (auto& o : g_clients) // aabb �浹üũ
    {
        if (o.second.m_id == npc_id)
            continue;
        if (ST_ACTIVE != o.second.m_status)
            continue;
        if (ST_DEAD == o.second.m_status)
            continue;
        check_aabb_collision(npc_id, o.second.m_id);
    }

    for (int i = 0; i < NPC_START; ++i) // npc �þ߹��� �� �ִ� �÷��̾�鿡�� ��ȣ ������ ��
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
                g_clients[i].m_cLock.unlock(); // ���� �Ƹ� �߸���������
                send_leave_packet(i, npc_id);
            }
            else
                g_clients[i].m_cLock.unlock();
        }
    }

    if (n.m_LastAnim != n.m_anim)
        do_animation(npc_id, n.m_anim);

    if (ST_ACTIVE == g_clients[npc_id].m_status)
        add_timer(npc_id, g_clients[npc_id].m_last_order, FRAME_TIME); // ���� ���� �ݺ� ����
}

void Server::add_timer(int obj_id, ENUM_FUNCTION op_type, int duration)
{
    timer_lock.lock();
    event_type event{ obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), 0 };
    timer_queue.push(event);
    //timer_queue.emplace(obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), 0);
    // �޸� ���� ���Ͼ�� �Ϸ��� ���� ��Ĵ�� �ߴµ� emplace�� ���ڸ� �ν��� ����, ���� ������ ���������� �̷����� �߻�
    timer_lock.unlock();
}

void Server::do_event_timer()
{
    while (true)
    {
        //Sleep(1); // �����쿡���� ������
        this_thread::sleep_for(1ms); // busy waiting ���� �� �ٸ� �����忡�� cpu �纸, 1�и��ʸ��� �˻��ض�, ��� �ϰ����� ����
        while (true) // ���� �ð��� �Ȱ� ������ ��� �������ִ� ��
        {
            timer_lock.lock();
            if (true == timer_queue.empty()) // Ÿ�̸� ť�� �ƹ��͵� ������
            {
                timer_lock.unlock();
                break; // ���� �ð��� �ȵ����� ���� ������ 1�� �����
            }
            if (timer_queue.top().wakeup_time > high_resolution_clock::now()) // ���� �ð��� �Ͼ�ð����� ������ ���� �Ͼ���� �ƴϴ�
                // ���� ť���� �������� �ƴϴ�           
            {
                // �ٵ� [ < busy waiting ]�̷��� ���������� : ������ ������ ������ �ݺ����� �����ϸ� ��ٸ��� ���> �߻���
                timer_lock.unlock();
                break; // ���� �ð��� �ȵ����� ���� ������ 1�� �����
            }
            event_type event = timer_queue.top(); // �̷��� �ϸ� �޸� ���� �Ͼ, ������� Ŀ��, �׷��� ť�� �ڷ����� �����ͷ� ������
            // �׷��� �ؼ� �߻��ϴ� new ��ü�� ������� �ƴϳ�, ����, �׷��� free list �Ἥ ���� ������� -> �˾Ƽ� �Ұ�
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

    //packet.move_time = g_clients[mover].m_move_time; // ��Ʈ���� �׽�Ʈ
    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
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
    g_clients[user_id].m_cLock.lock(); // name, m_status ��
    strcpy_s(g_clients[user_id].m_name, name);
    g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // �������� NULL �־��ִ� ó��
    send_login_ok_packet(user_id); // ���� ������ �÷��̾� �ʱ�ȭ ���� ������
    g_clients[user_id].m_status = ST_ACTIVE; // �ٸ� Ŭ������� ���� ���� ������ �������� ST_ACTIVE�� �ٲ��ֱ�
    g_clients[user_id].m_cLock.unlock();
    for (int i = 0; i < 5; ++i)
        send_flag_info_packet(i, user_id); // ���� ������ �÷��̾� �ʱ�ȭ ���� ������
    cout << "Player " << user_id << " login finish" << endl;
    for (int i = 0; i <= MAX_USER; ++i)
    {
        if (user_id == i) // ����� ȸ�ǿ�
            continue;

        if (true == is_near(user_id, i))
        {
            g_clients[i].m_cLock.lock();
            if (ST_SLEEP == g_clients[i].m_status)
            {
                g_clients[i].m_cLock.unlock();
                activate_npc(i, FUNC_NPC_HOLD);
            }
            else if (ST_ACTIVE == g_clients[i].m_status) // �̹� ���� ���� Ŭ������׸�, m_status�� ���� �ɾ�� ������
            {
                g_clients[i].m_cLock.unlock();
                send_enter_packet(user_id, i); // ���� ������ Ŭ�󿡰� �̹� �������� Ŭ�� �������� ���� 
                if (true == is_player(i))
                    send_enter_packet(i, user_id); // �̹� ������ �÷��̾�鿡�� ���� ������ Ŭ������ ����
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
                //add_timer(-1, FUNC_CHECK_FLAG, 100);// ���� �÷��� �ð� ������ �Լ�
                add_timer(PURE_EVENT, FUNC_CHECK_TIME, 1000);// ���� �÷��� �ð� ������ �Լ�
                add_timer(PURE_EVENT, FUNC_CHECK_GOLD, 1000);// ���� �÷��� �ð� ������ �Լ�
                add_timer(PURE_EVENT, FUNC_CHECK_COLLISION, FRAME_TIME);// ���� �÷��� �ð� ������ �Լ�
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
        g_clients[i].m_id = i; // ���� ���
        g_clients[i].m_status = ST_FREE; // ����� ��Ƽ������ �ϱ����� �̱۽������϶� ����ϴ� �Լ�, �� ���ʿ�
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
            for (int i = 0; i < NPC_START; ++i) // �ٸ� �÷��̾��߿� �þ߹��� �ȿ��ִ� �÷��̾�� ���� ���� npc ���̰��ϱ�
            {
                if (true == is_near(npc_id, i))
                {
                    if (ST_ACTIVE == g_clients[i].m_status) // �̹� ���� ���� Ŭ������׸�, m_status�� ���� �ɾ�� ������
                    {
                        if (true == is_player(i))
                            send_enter_packet(i, npc_id); // �̹� ������ �÷��̾�鿡�� ���� ������ Ŭ������ ����
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
            continue; // ���� �����Ұ� (�ӽù���), �÷��̾��� ��� npc active�϶� ���̻� �߰� �ȵǰ� ó��
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

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_hp_packet(int user_id, int other_id)
{
    sc_packet_hp packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_HP;
    packet.id = other_id;
    packet.hp = g_clients[other_id].m_hp;

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_arrow_packet(int arrow_id, int user_id, int other_id)
{
    sc_packet_arrow packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ARROW;
    packet.shoot_id = other_id;
    packet.arrow_id = arrow_id;

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
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

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_do_particle_packet(int user_id, int other_id)
{
    sc_packet_do_particle packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_DO_PARTICLE;
    packet.id = other_id;

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_npc_size_packet(int user_id)
{
    sc_packet_npc_size packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_NPC_SIZE;
    packet.npc_size = g_clients[user_id].m_boid.size();
    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_class_change_packet(int user_id, int other_id)
{
    sc_packet_class_change packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_CLASS_CHANGE;
    packet.id = other_id;
    packet.p_class = g_clients[other_id].m_class;
    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
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
    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_invisible_packet(int user_id, int other_id, bool invi)
{
    sc_packet_invisible packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_INVISIBLE;
    packet.id = other_id;
    packet.invisable = invi;
    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_fire_packet(int id, float mx, float mz)
{
    sc_packet_fire packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_FIRE;
    packet.x = mx;
    packet.z = mz;
    send_packet(id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_teleport_packet(int id, float mx, float mz, unsigned char m)
{
    sc_packet_teleport packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_TELEPORT;
    packet.mode = m;
    packet.x = mx;
    packet.z = mz;
    send_packet(id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
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

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::do_animation(int user_id, unsigned char anim)
{
    g_clients[user_id].m_LastAnim = anim;
    //cout << user_id << "is " << g_clients[user_id].m_anim << endl;
    if (user_id < NPC_START) // �ִϸ��̼� ��ȣ�� ������ �ְ� �÷��̾��
    {
        g_clients[user_id].m_cLock.lock();
        unordered_set<int> copy_viewlist = g_clients[user_id].m_view_list;
        g_clients[user_id].m_cLock.unlock();
        send_animation_packet(user_id, user_id, anim); // �ӽ�
        for (auto cpy_vl : copy_viewlist) // ������ ������ �þ� ������ ���Ͽ�
        {
            if (is_player(cpy_vl)) // �ٸ� �÷��̾� ���Ը�
                send_animation_packet(cpy_vl, user_id, anim); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
        }
    }
    else if (user_id >= NPC_START && user_id <= MAX_NPC) // 
    {
        for (int i = 0; i < NPC_START; ++i) // ��� �÷��̾ ���ؼ�
        {
            if (false == is_near(i, user_id)) // ��ó�� ���� ������ �������� ����
                continue;
            if (ST_ACTIVE != g_clients[i].m_status) // �α��� ���� �ƴѾָ� ������ ����
                continue;

            send_animation_packet(i, user_id, anim); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
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
    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::do_attack(int npc_id)
{
    SESSION& n = g_clients[npc_id];
    _vec3 n_look = *n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    n_look = -1.f * Vector3_::Normalize(n_look);
    _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    if (!n.m_isOBB) // OBB �浹���� ���°� �ƴҶ�
    {
        if (n.m_attack_target < 0) // ������ ����� �������� �ʾ�����
        {
            for (int i = 0; i < OBJECT_START; ++i) // ���� �ֺ��� ���� �ֳ� Ȯ��
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
            else // �ֺ��� ���� ������ Ž�� ������ ����
            {
                _vec3 t_look = g_clients[npc_id].m_target_look;
                t_look = -1.f * Vector3_::Normalize(t_look);

                float PdotProduct = (n_look.x * t_look.x) + (n_look.y * t_look.y) + (n_look.z * t_look.z); // ����
                float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

                float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // �տ� x ���� ���� ���� ����
                if (PoutProduct > 0) // ����̸� n_look�� t_look�� ���� �ݽð�
                    radian *= -1.f;

                float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����

                if (NPCangle > 3.f || NPCangle < -3.f) // npc�� �ٶ󺸴� ������ �÷��̾�� ��ġ���� ������
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
                else // npc�� �ٶ󺸴� ������ �÷��̾�� ��ġ�Ҷ�
                {
                    n.m_anim = A_WALK;
                    n.m_transform.BackWard(TIME_DELTA * 2.f);
                }
            }
        }
        else // ������ ����� ������ �Ǿ�����
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

            float PdotProduct = (n_look.x * t_look.x) + (n_look.y * t_look.y) + (n_look.z * t_look.z); // ����
            float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

            float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // �տ� x ���� ���� ���� ����
            if (PoutProduct > 0) // ����̸� n_look�� t_look�� ���� �ݽð�
                radian *= -1.f;

            float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����

            if (NPCangle > 3.f || NPCangle < -3.f) // npc�� ������ ����� �ȹٶ󺼶�
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
            else // npc�� ������ ����� �ٶ󺼶�
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
                        if (g_clients[n.m_attack_target].m_hp > 0) // ������ ���������
                        {
                            for (int i = 0; i < NPC_START; ++i)
                            {
                                if (ST_ACTIVE != g_clients[i].m_status)
                                    continue;
                                if (!is_near(i, n.m_attack_target))
                                    continue;
                                send_do_particle_packet(i, n.m_attack_target); // ���� ü�� ��ε�ĳ����
                                send_hp_packet(i, n.m_attack_target); // ���� ü�� ��ε�ĳ����
                            }
                        }
                        else // ������ �׾�����
                        {
                            do_dead(n.m_attack_target);
                            n.m_attack_target = -1;
                        }
                    }
                    n.m_anim = A_ATTACK;
                }
                else // OBB ���ݹ��� ���϶�
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
        send_do_particle_packet(i, id); // ���� ü�� ��ε�ĳ����
        send_hp_packet(i, id); // ���� ü�� ��ε�ĳ����
        send_dead_packet(i, id); // ���̰� ���� ü�� ������
        send_animation_packet(i, id, A_DEAD);
    }
    if (is_player(id))
    {
        // �״� �ִϸ��̼� ������ �÷��̾�� Ŭ�� �ִϸ��̼� �������� ��Ȱ��ȣ ������ ��Ȱ
    }
    else
    {
        // �״� �ִϸ��̼� ������ npc�� send_leave_packet(i, id); // ���� ü�� ��ε�ĳ����
    }
}

void Server::disconnect(int user_id)
{
    //send_leave_packet(user_id, user_id); // �� �ڽ�
    cout << user_id << "is disconnect\n";
    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_status = ST_ALLOC; // ���⼭ free �ع����� �Ʒ����� �����߿� �ٸ� Ŭ�� �Ҵ�ɼ��� ����
    closesocket(g_clients[user_id].m_socket);

    for (int i = 0; i < NPC_START; ++i)
    {
        if (user_id == i)
            continue;
        if (ST_ACTIVE == g_clients[i].m_status)
        {
            send_leave_packet(i, user_id); // ������ send_leave_packet ���ο��� �丮��Ʈ ���� ���༭ ���⿡ ���� �� �ʿ�X
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
    g_clients[user_id].m_status = ST_FREE; // ��� ó���� ������ free�ؾ���
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
        // abs = ���밪
        return false;
    // �̰� 2D �����̴ϱ� ����� �������� �� �簢���̹Ƿ� �簢�� �������� �þ߹��� ���
    // 3D ������ ��Ʈ(x-x�� ���� + y-y�� ����)> VIEW_RADIUS �̸� false�� ó���ؾ���
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

    float PdotProduct = (at_look.x * t_look.x) + (at_look.y * t_look.y) + (at_look.z * t_look.z); // ����
    float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

    float PoutProduct = (at_look.x * t_look.z) - (at_look.z * t_look.x); // �տ� x ���� ���� ���� ����
    if (PoutProduct > 0) // ����̸� t_look�� at_look�� ���� �ݽð�
        radian *= -1.f;

    float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����

    if (NPCangle > 45.f || NPCangle < -45.f) // npc�� ������ ����� �ȹٶ󺼶�
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
        if (ST_ACTIVE != g_clients[player].m_status) // ������ ������ �ֵ� ����
            continue;

        _vec3* a_pos = &flags[flag].pos; // ���
        _vec3* b_pos = g_clients[player].m_transform.Get_StateInfo(CTransform::STATE_POSITION); // �÷��̾�

        if (sqrt((a_pos->x - b_pos->x) *
            (a_pos->x - b_pos->x) +
            (a_pos->y - b_pos->y) *
            (a_pos->y - b_pos->y) +
            (a_pos->z - b_pos->z) *
            (a_pos->z - b_pos->z)) < FLAG_RADIUS) // ��� ���� ���̸�
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
                        if (ST_ACTIVE != g_clients[j].m_status) // ������ ������ �ֵ� ����
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
                        if (ST_ACTIVE != g_clients[j].m_status) // ������ ������ �ֵ� ����
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
                if (ST_ACTIVE != g_clients[j].m_status) // ������ ������ �ֵ� ����
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
                if (ST_ACTIVE != g_clients[j].m_status) // ������ ������ �ֵ� ����
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
    //{ // ��Ʈ���� �׽�Ʈ
    //    _vec3 pos = { (float)(rand()%WORLD_HORIZONTAL), 0.f, (float)(rand() % WORLD_HORIZONTAL) };
    //    g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
    //}

    //flags[0].pos = { 50.f, 0.2f, 50.f };
    //flags[1].pos = { 100.f, 0.2f, 450.f };
    //flags[2].pos = { 250.f, 0.2f, 250.f };
    //flags[3].pos = { 450.f, 0.2f, 400.f };
    //flags[4].pos = { 450.f, 0.2f, 100.f };
    //���ϱ� 7.5�� / �ʻ�����  3750 - 3750
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
                send_hp_packet(j, i); // ���� ü�� ��ε�ĳ����
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
                // Ȱ��ȭ �Ǿ��ְ� ������ �þ߹��� �ȿ� �ִ� �����϶�
                send_leave_packet(j, i); // ���� ü�� ��ε�ĳ����
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
        GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE); // recv ��� IOCP�� ����
        // io_byte�� 0�� ��� = �� Ŭ�� ������ �����߱� ������ 0����Ʈ�� ���۵ȴ�
        OverEx* overEx = reinterpret_cast<OverEx*>(over); // �ӽ� Ȯ�� ������ ����ü�� IOCP�� ����� �� ����
        int id = static_cast<int>(key); // �ӽ� ���̵� IOCP Ű��(Ŭ�� id��) ����

        switch (overEx->function) // send, recv, accept ����
        {
        case FUNC_RECV:
        {
            if (0 == io_byte)
                disconnect(id);
            else
            {
                recv_packet_construct(id, io_byte);
                ZeroMemory(&g_clients[id].m_recv_over.over, sizeof(g_clients[id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
                DWORD flags = 0;
                WSARecv(g_clients[id].m_socket, &g_clients[id].m_recv_over.wsabuf, 1, NULL, // ��Ŷ ó���� �ʱ�ȭ ������ �ٽ� recv ȣ��
                    &flags, &g_clients[id].m_recv_over.over, NULL);
            }
        }
        break;

        case FUNC_SEND:
            if (0 == io_byte)
                disconnect(id);

            delete overEx; // Ȯ�� ����ü �ʱ�ȭ�� ���ָ� �ȴ�
            break;

        case FUNC_ACCEPT:
        {
            // ����� �� �����尡 accept ������ ���� AcceptEx�� ȣ���ϰ� �س��� ���� �����尡 ���� ���� ����
            int user_id = -1;
            for (int i = 0; i <= MAX_USER; ++i)
            {
                lock_guard <mutex> guardLock{ g_clients[i].m_cLock }; // �Լ����� lock �Ҷ� ����
                // �� cLock�� ���� �ɰ� �����尡 ���� ��Ͽ��� ���������� unlock���ְ� ���� �������� unlock-lock ����
                if (ST_FREE == g_clients[i].m_status) // ���� ��ü ���鼭 ���� �����Ѿ� id �ο�
                {
                    g_clients[i].m_status = ST_ALLOC;
                    user_id = i;
                    break; // �Ҵ� ������ ��ü ������ break
                }
            }

            SOCKET clientSocket = overEx->c_socket;

            if (-1 == user_id) // ���� ��� ��ü ���Ҵµ� �Ҵ� ������ ��ü�� ������
                closesocket(clientSocket); //send_login_fail_packet();
            else
            {
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, user_id, 0); // IOCP ��ü�� �������� ��� �� ��ü �ʱ�ȭ

                g_clients[user_id].m_prev_size = 0; // ���� 0���� �ʱ�ȭ
                g_clients[user_id].m_recv_over.function = FUNC_RECV; // ������ ����ü�� �޴°ɷ� ����
                ZeroMemory(&g_clients[user_id].m_recv_over.over, sizeof(g_clients[user_id].m_recv_over.over)); // ������ ����ü �ʱ�ȭ
                g_clients[user_id].m_recv_over.wsabuf.buf = g_clients[user_id].m_recv_over.io_buf; // WSA ���� ��ġ ����
                g_clients[user_id].m_recv_over.wsabuf.len = MAX_BUF_SIZE; // WSA���� ũ�� ����
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
                g_clients[user_id].m_total_angle = -90.f; // ����
                g_clients[user_id].m_LastAnim = A_IDLE;
                g_clients[user_id].m_anim = A_IDLE;
                g_clients[user_id].m_troop = T_ALL;
                g_clients[user_id].m_owner_id = user_id; // ���� ���
                g_clients[user_id].m_view_list.clear(); // ���� �丮��Ʈ ������ ������ �ȵǴ� �ʱ�ȭ
                g_clients[user_id].m_isAttack = false;
                g_clients[user_id].m_isOBB = false;
                set_starting_pos(user_id);

                DWORD flags = 0;
                WSARecv(clientSocket, &g_clients[user_id].m_recv_over.wsabuf, 1, NULL,
                    &flags, &g_clients[user_id].m_recv_over.over, NULL); // ������� �ϳ��� Ŭ�� ���� ����̶� recv ȣ���� ������
            }
            // ���⼭���� ���ο� Ŭ�� ���� accept
            clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // ���� ���� Ŭ�� ���� 
            overEx->c_socket = clientSocket; // ���� ���� Ŭ��ϱ� �� ���� ������ Ȯ�� �������� �־������
            ZeroMemory(&overEx->over, sizeof(overEx->over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
            // accept�� �Ϸ�� ������ �ٽ� accept �޴� �κ��̹Ƿ� overEx �ٽ� ����ص���, �ߺ��ؼ� �Ҹ� �� ����
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
                for (auto& it : g_clients) // OBB �۵��ѳ� OBB��Ű��
                {
                    if (ST_ACTIVE != it.second.m_status && ST_DEAD != it.second.m_status) // Ȱ��ȭ �ȳ�, �׾��ִ³��� ����
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

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // �׳� ���ε� ���� C++11�� �ִ� �����Լ��� ����
    listen(listenSocket, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // Ŀ�� ��ü ����, IOCP ��ü ����
    initialize_objects(); // ������Ʈ ������ �ʱ�ȭ
    ready_timer();
    isGameStart = false;

    // �񵿱� accept�� �ϷḦ �޾ƾ��� -> iocp�� �޾ƾ��� -> ���� ��Ĺ�� ����������
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), g_iocp, LISTEN_KEY, 0); // ���� ��Ĺ iocp ��ü�� ���
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OverEx accept_over;
    ZeroMemory(&accept_over.over, sizeof(accept_over.over)); // accept�� Ȯ�� ������ ����ü �ʱ�ȭ
    accept_over.function = FUNC_ACCEPT;
    accept_over.c_socket = clientSocket; // clientSocket�� worker_thread�� �����������
    // accept���� wsabuf �̿���ϹǷ� �ʱ�ȭ �� �ʿ� ����
    AcceptEx(listenSocket, clientSocket, accept_over.io_buf, NULL,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);

    vector<thread> worker_threads;
    for (int i = 0; i < 4; ++i) // ���⿡ �����ھ�� 4 �־��µ� ���� �ھ����ŭ �־ ����
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

        for (int i = 0; i < NPC_START; ++i) // �и� ���Ѿ� ��ǥ�� ������Ʈ
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
    if (arrow_life.count() < ARROW_ENDTIME) // ���� ȭ�� �����ð� ����������
    {
        g_clients[arrow_id].m_transform.Go_Right(TIME_DELTA);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            send_move_packet(i, arrow_id); // ���� ü�� ��ε�ĳ����
        }
        do_arrow_collision(arrow_id);
        if (ST_ACTIVE == g_clients[arrow_id].m_status)
            add_timer(arrow_id, FUNC_ARROW, FRAME_TIME);
    }
    else // ȭ�� �����ð� ��������
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
            if (arrow == iter.first) // �ڱ� �ڽ� ����
                continue;
            if (g_clients[arrow].m_owner_id == iter.first) // ȭ�� �� ���� ����
                continue;
            if (TP_ARROW == iter.second.m_type || TP_SKILL == iter.second.m_type) // ȭ��, ��ų�̶� �浹 ����
                continue;

            // ������ �÷��̾�, npc, �ڿ���, deffend ���� �浹
            _float fLength = 0.f;
            _vec3 arrow_Pos = *g_clients[arrow].m_transform.Get_StateInfo(CTransform::STATE_POSITION); // Arrow
            _vec3 gothit_Pos = *iter.second.m_transform.Get_StateInfo(CTransform::STATE_POSITION); // other

            _vec3 vDistance = gothit_Pos - arrow_Pos;
            fLength = vDistance.Length();

            if (fLength <= ARROW_DIST)
            {
                if (iter.second.m_type == TP_NATURE || iter.second.m_type == TP_DEFFEND) // ȭ�� �����ְ� �и��� �ȵ�
                {
                    iter.second.m_hp -= ARROW_DAMAGE;
                    for (int i = 0; i < NPC_START; ++i)
                    {
                        if (ST_ACTIVE != g_clients[i].m_status)
                            continue;
                        if (!is_near(i, iter.first))
                            continue;
                        send_do_particle_packet(i, iter.first); // ���� ü�� ��ε�ĳ����
                        send_hp_packet(i, iter.first); // ���� ü�� ��ε�ĳ����
                    }
                    delete_arrow(arrow_id);
                }
                else // �÷��̾�,npc,������ ������ obb �з�����
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
                        send_do_particle_packet(i, iter.first); // ���� ü�� ��ε�ĳ����
                        send_hp_packet(i, iter.first); // ���� ü�� ��ε�ĳ����
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
