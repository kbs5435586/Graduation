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

void Server::start_game()
{
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

void Server::process_packet(int user_id, char* buf)
{
    switch (buf[1]) // �츮�� ��Ŷ�� 0�� char size, 1�� char type���� ���������Ƿ�
    {
    case CS_PACKET_LOGIN: // case������ ������ �����Ҷ� �߰�ȣ ����� ������ ������ �ȴ�
    {
        cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
        enter_game(user_id, packet->name); // ���� �������� ���� ó�� �� �̹� �����ִ� Ŭ�� ���� ����

        // ���� �� �ɶ� socket�̶� connected�� name�� ���ÿ� ��ȣ�������
        // bool connected ���� ������ true�϶� socket�̳� nameó�� �ѹ� ���� ������Ʈ ���� �ʴ�
        // ������ �����ϴٴ� �ǹ�, �׷��� connected�� true �Ǳ� ���� socket, name�� ó���ϴ���
        // true�Ҷ� ���� ���� �ɴ��� �� �� �ϳ��� �ؾ���
    }
    break;
    case CS_PACKET_CONDITION:
    {
        cs_packet_condition* packet = reinterpret_cast<cs_packet_condition*>(buf);
        // g_clients[user_id].m_move_time = packet->move_time; // ��Ʈ���� �׽�Ʈ
        if (CON_TYPE_MOVE == packet->con_type)
        {
            //if (CON_IDLE == packet->con)
            //    cout << user_id << " send move condition : CON_IDLE\n";
            //else if (CON_STRAIGHT == packet->con)
            //    cout << user_id << " send condition : CON_STRAIGHT\n";
            //else if (CON_BACK == packet->con)
            //    cout << user_id << " send condition : CON_BACK\n";
            do_move(user_id, packet->con);
        }
        else if (CON_TYPE_ROTATE == packet->con_type)
        {
            //if (CON_IDLE == packet->con)
            //    cout << user_id << " send rote condition : CON_IDLE\n";
            //else if (CON_LEFT == packet->con)
            //    cout << user_id << " send condition : CON_LEFT\n";
            //else if (CON_RIGHT == packet->con)
            //    cout << user_id << " send condition : CON_RIGHT\n";
            do_rotate(user_id, packet->con);
        }
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
            if (!is_attack_view(user_id, i))
                continue;

            g_clients[user_id].m_attack_target = i;
            do_battle(user_id);
        }
    }
    break;
    case CS_PACKET_ANIMATION:
    {
        cs_packet_animation* packet = reinterpret_cast<cs_packet_animation*>(buf);
        do_animation(user_id, packet->anim);
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
            update_npc_troop(recv_id);
        update_speed(recv_id);

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
        if (ST_ACTIVE != g_clients[i].m_status) // ������ ������ �ֵ� ����
            continue;

        send_packet(i, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
    }
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

void Server::do_rotate(int user_id, char con)
{
    SESSION& c = g_clients[user_id];
    c.m_transform.SetUp_Speed(c.m_move_speed, c.m_rotate_speed);

    if (CON_IDLE == con)
    {
        c.m_cLock.lock();
        c.m_curr_rotate = FUNC_PLAYER_IDLE;
        c.m_cLock.unlock();
        c.m_Rcondition = CON_IDLE;
    }
    else if (CON_RIGHT == con)
    {
        c.m_curr_rotate = FUNC_PLAYER_RIGHT;
        c.m_Rcondition = CON_RIGHT;
        //c.m_transform.Rotation_Y(ROTATE_SPEED);
        add_timer(user_id, FUNC_PLAYER_RIGHT, FRAME_TIME);
    }
    else if (CON_LEFT == con)
    {
        c.m_curr_rotate = FUNC_PLAYER_LEFT;
        c.m_Rcondition = CON_LEFT;
        //c.m_transform.Rotation_Y(-ROTATE_SPEED);
        add_timer(user_id, FUNC_PLAYER_LEFT, FRAME_TIME);
    }

    c.m_cLock.lock();
    unordered_set<int> copy_viewlist = c.m_view_list;
    // ���纻 �丮��Ʈ�� �ٸ� �����尡 �����ϸ� ��¼��? �� ������ �����ؾ���
    c.m_cLock.unlock();

    send_condition_packet(user_id, user_id, CON_TYPE_ROTATE); // ���� ���ư��ٴ� ���� ������, �ڿ��� ������ ���ư���, �ϴ� ���� �� ���ư��� �˸�
    send_fix_packet(user_id, user_id);
    for (auto cpy_vl : copy_viewlist) // ������ ������ �þ� ������ ���Ͽ�
    {
        if (is_player(cpy_vl))
        {
            send_condition_packet(cpy_vl, user_id, CON_TYPE_ROTATE); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
            send_fix_packet(cpy_vl, user_id);
        }
    }
}

void Server::update_npc_troop(int npc_id)
{
    SESSION& c = g_clients[npc_id];

    if (C_WORKER == c.m_class)
        c.m_troop = T_INFT;
    if (C_CAVALRY == c.m_class)
        c.m_troop = T_INFT;
    if (C_TWO == c.m_class)
        c.m_troop = T_INFT;
    if (C_INFANTRY == c.m_class)
        c.m_troop = T_HORSE;
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

void Server::update_speed(int user_id)
{
    SESSION& c = g_clients[user_id];

    if (C_WORKER == c.m_class)
    {
        c.m_move_speed = 10.f;
    }
    if (C_CAVALRY == c.m_class)
    {
        c.m_move_speed = 20.f;
    }
    if (C_TWO == c.m_class)
    {
        c.m_move_speed = 20.f;
    }
    if (C_INFANTRY == c.m_class)
    {
        c.m_move_speed = 10.f;
    }
    if (C_FOUR == c.m_class)
    {
        c.m_move_speed = 10.f;
    }
    if (C_SPEARMAN == c.m_class)
    {
        c.m_move_speed = 10.f;
    }
    if (C_MAGE == c.m_class)
    {
        c.m_move_speed = 10.f;
    }
    if (C_MMAGE == c.m_class)
    {
        c.m_move_speed = 20.f;
    }
    if (C_ARCHER == c.m_class)
    {
        c.m_move_speed = 15.f;
    }
}

void Server::do_move(int user_id, char con)
{
    SESSION& c = g_clients[user_id];

    update_speed(user_id);
    c.m_transform.SetUp_Speed(c.m_move_speed, c.m_rotate_speed);

    switch (con)
    {
    case CON_IDLE:
    {
        c.m_cLock.lock();
        c.m_curr_move = FUNC_PLAYER_IDLE;
        c.m_cLock.unlock();
        c.m_Mcondition = CON_IDLE;
    }
    break;
    case CON_STRAIGHT:
        //if (newpos->z >= 0 || newpos->x >= 0)
    {
        c.m_curr_move = FUNC_PLAYER_STRAIGHT;
        c.m_Mcondition = CON_STRAIGHT;
        //c.m_transform.BackWard(MOVE_SPEED_PLAYER);
        add_timer(user_id, FUNC_PLAYER_STRAIGHT, FRAME_TIME);
    }
    break;
    case CON_RUN:
        //if (newpos->z >= 0 || newpos->x >= 0)
    {
        c.m_curr_move = FUNC_PLAYER_RUN;
        c.m_Mcondition = CON_RUN;
        //c.m_transform.BackWard(MOVE_SPEED_PLAYER * 2.f);
        add_timer(user_id, FUNC_PLAYER_RUN, FRAME_TIME);
    }
    break;
    case CON_BACK:
        //if (newpos->z < WORLD_VERTICAL || newpos->x < WORLD_HORIZONTAL)
    {
        c.m_curr_move = FUNC_PLAYER_BACK;
        c.m_Mcondition = CON_BACK;
        //c.m_transform.Go_Straight(MOVE_SPEED_PLAYER);
        add_timer(user_id, FUNC_PLAYER_BACK, FRAME_TIME);
    }
    break;
    default:
        cout << "Unknown Direction From cs_move_packet !\n";
        DebugBreak();
        exit(-1);
    }

    c.m_cLock.lock();
    unordered_set<int> copy_viewlist = c.m_view_list;
    // ���纻 �丮��Ʈ�� �ٸ� �����尡 �����ϸ� ��¼��? �� ������ �����ؾ���
    c.m_cLock.unlock();

    send_condition_packet(user_id, user_id, CON_TYPE_MOVE); // ���� ���ư��ٴ� ���� ������, �ڿ��� ������ ���ư���, �ϴ� ���� �� ���ư��� �˸�
    send_fix_packet(user_id, user_id);
    for (auto cpy_vl : copy_viewlist) // ������ ������ �þ� ������ ���Ͽ�
    {
        if (is_player(cpy_vl))
        {
            send_condition_packet(cpy_vl, user_id, CON_TYPE_MOVE); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
            send_fix_packet(cpy_vl, user_id);
        }
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
    //case FM_SQUARE:
    //{
    //    if (1 == c.m_boid.size())
    //    {
    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Left(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[0]->m_target_pos = *new_pos;
    //    }
    //    else if (2 == c.m_boid.size())
    //    {
    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Left(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[0]->m_target_pos = *new_pos1;

    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Right(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[1]->m_target_pos = *new_pos2;
    //    }
    //    else if (3 == c.m_boid.size())
    //    {
    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Left(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[0]->m_target_pos = *new_pos1;

    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Right(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[1]->m_target_pos = *new_pos2;

    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Left(FORMATION_SPACE);
    //        set_pos.Go_Straight(FORMATION_SPACE);
    //        _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[2]->m_target_pos = *new_pos3;
    //    }
    //    else if (4 == c.m_boid.size())
    //    {
    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Left(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[0]->m_target_pos = *new_pos1;

    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Right(FORMATION_SPACE);
    //        set_pos.BackWard(FORMATION_SPACE);
    //        _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[1]->m_target_pos = *new_pos2;

    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Left(FORMATION_SPACE);
    //        set_pos.Go_Straight(FORMATION_SPACE);
    //        _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[2]->m_target_pos = *new_pos3;

    //        set_pos.Set_Matrix(&temp);
    //        set_pos.Go_Right(FORMATION_SPACE);
    //        set_pos.Go_Straight(FORMATION_SPACE);
    //        _vec3* new_pos4 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
    //        c.m_boid[3]->m_target_pos = *new_pos4;
    //    }
    //}
    //break;
    case F_PIRAMID:
    {

    }
    break;
    case F_CIRCLE:
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
                //send_move_packet(i, npc_id);
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

    // �ٵ� ���� �÷��̾ ������, �÷��̾� �� ����Ʈ �����Ҷ� npc���� ����ؼ� �� ����Ʈ ��������� �ǹǷ� ó������ ������ �丮��Ʈ �� ���������
}

void Server::do_follow(int npc_id)
{
    SESSION& n = g_clients[npc_id];
    for (int i = 0; i < g_clients[n.m_owner_id].m_boid.size(); ++i)
    {
        if (g_clients[n.m_owner_id].m_boid[i].id == n.m_id)
        {
            _vec3* n_pos = n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3* n_look = n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
            _vec3* p_pos = g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3* p_look = g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
            if (*n_pos != g_clients[n.m_owner_id].m_boid[i].final_pos) // ���� �ڽ��� ���� ��ġ�� �ƴҶ�
            {
                if (dist_between(npc_id, n.m_owner_id) > dist_between_finalPos(n.m_owner_id, i) + 0.1f
                    || dist_between(npc_id, n.m_owner_id) < dist_between_finalPos(n.m_owner_id, i) - 0.1f)// �ڽ��� �����̼� ������ ���϶�
                {
                    n.m_anim = A_WALK;
                    n.m_Mcondition = CON_STRAIGHT;
                    n.m_transform.BackWard(MOVE_TIME_ELAPSE);
                    _vec3 finalLookAt = g_clients[n.m_owner_id].m_boid[i].final_pos - *n_pos;
                    finalLookAt = Vector3_::Normalize(finalLookAt);
                    _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look);

                    _vec3 set_pos = {};

                    float PdotProduct = (npcLookAt.x * finalLookAt.x) + (npcLookAt.y * finalLookAt.y) + (npcLookAt.z * finalLookAt.z); // ����
                    _vec3 PoutProduct;
                    PoutProduct.x = (finalLookAt.y * npcLookAt.z) - (finalLookAt.z * npcLookAt.y); // �ܰ�
                    PoutProduct.y = (finalLookAt.z * npcLookAt.x) - (finalLookAt.x * npcLookAt.z);
                    PoutProduct.z = (finalLookAt.x * npcLookAt.y) - (finalLookAt.y * npcLookAt.x);

                    float radian = acosf(PdotProduct); // ���� �̿��� ���� ����
                    if (PoutProduct.y < 0)
                        radian *= -1.f;
                    float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
                    if (NPCangle > 0)
                    {
                        n.m_Rcondition = CON_LEFT;
                        n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
                    }
                    else if (NPCangle < 0)
                    {
                        n.m_Rcondition = CON_RIGHT;
                        n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);
                    }
                    else
                    {
                        n.m_Rcondition = CON_IDLE;
                    }
                    n.m_isOut = true;
                    n.m_isFormSet = false;
                }
                else // �ڽ��� �����̼� �������� ���ų� �̳��϶�
                {
                    if (n.m_isOut) // ���ʷ� ������ ������ ��������
                    {
                        n.m_isOut = false;
                        n.m_Mcondition = CON_IDLE;
                        n.m_Rcondition = CON_IDLE;
                        _vec3 standard = -1.f * Vector3_::Normalize(*p_look);
                        _vec3 toNpc = *n_pos - *p_pos;
                        toNpc = Vector3_::Normalize(toNpc);

                        _vec3 set_pos = {};

                        float PdotProduct = (toNpc.x * standard.x) + (toNpc.y * standard.y) + (toNpc.z * standard.z); // ����
                        _vec3 PoutProduct;
                        PoutProduct.x = (standard.y * toNpc.z) - (standard.z * toNpc.y); // �ܰ�
                        PoutProduct.y = (standard.z * toNpc.x) - (standard.x * toNpc.z);
                        PoutProduct.z = (standard.x * toNpc.y) - (standard.y * toNpc.x);

                        float radian = acosf(PdotProduct); // ���� �̿��� ���� ����
                        if (PoutProduct.y < 0)
                            radian *= -1.f;
                        float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
                        if (NPCangle > 0)  NPCangle += 3.f;
                        else if (NPCangle < 0) NPCangle -= 3.f;
                        n.m_total_angle = g_clients[n.m_owner_id].m_total_angle + NPCangle;
                        n_pos->x = g_clients[n.m_owner_id].m_boid[i].radius * sinf((n.m_total_angle) * (PIE / 180.f)) + p_pos->x;
                        n_pos->z = g_clients[n.m_owner_id].m_boid[i].radius * cosf((n.m_total_angle) * (PIE / 180.f)) + p_pos->z;
                        {
                            for (int i = 0; i < NPC_START; ++i) // npc �þ߹��� �� �ִ� �÷��̾�鿡�� ��ȣ ������ ��
                            {
                                if (ST_ACTIVE != g_clients[i].m_status)
                                    continue;
                                if (true == is_near(npc_id, i))
                                {
                                    send_fix_packet(i, npc_id);
                                }
                            }
                        }
                    }
                    else if (!n.m_isOut && !n.m_isFormSet) // ���ʷ� ������ ���°� �ƴѵ� ������� �ȵǾ�������
                    {
                        n.m_Mcondition = CON_IDLE;
                        _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look); // npc�� �ٶ󺸴� ����

                        _vec3 toNpc = *n_pos - *p_pos; // �÷��̾�� npc �ٶ󺸴� ����
                        toNpc = Vector3_::Normalize(toNpc);

                        float PdotProduct = (npcLookAt.x * toNpc.x) + (npcLookAt.y * toNpc.y) + (npcLookAt.z * toNpc.z); // ����
                        float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

                        float PoutProduct = (toNpc.x * npcLookAt.z) - (toNpc.z * npcLookAt.x); // �տ� x ���� ���� ���� ����
                        if (PoutProduct > 0) // ����̸� npcLookAt�� toNpc�� ���� �ݽð�
                            radian *= -1.f;

                        float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
                        if (-0.03f > PdotProduct || 0.03f < PdotProduct) // ���ư����� ���� ������ �ȹٶ󺼶�
                        {
                            if (g_clients[n.m_owner_id].m_boid[i].angle > n.m_total_angle) // ���� npc �������� ������ �����̼� ������ �� Ŭ��, ���̰� �÷���
                            {
                                n.m_Rcondition = CON_LEFT;
                                n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE); // ����ȸ��
                            }
                            else //���̰� ���̳ʽ�
                            {
                                n.m_Rcondition = CON_RIGHT;
                                n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE); // ������
                            }
                        }
                        else // ���ư����� ���� ������ �ٶ󺼶�
                        {
                            n.m_Rcondition = CON_IDLE;
                            n.m_isFormSet = true;
                            for (int i = 0; i < NPC_START; ++i) // npc �þ߹��� �� �ִ� �÷��̾�鿡�� ��ȣ ������ ��
                            {
                                if (ST_ACTIVE != g_clients[i].m_status)
                                    continue;
                                if (true == is_near(npc_id, i))
                                {
                                    send_fix_packet(i, npc_id);
                                }
                            }
                        }
                    }
                    else // ���� ���ñ��� �� �Ϸ� �Ǿ�����
                    {
                        if (g_clients[n.m_owner_id].m_boid[i].angle + 3.f < n.m_total_angle
                            || g_clients[n.m_owner_id].m_boid[i].angle - 3.f > n.m_total_angle) // �ڱⰡ �־���� ���� ��ġ�� �ƴҰ��
                        {
                            _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look); // npc�� �ٶ󺸴� ����

                            _vec3 toNpc = *n_pos - *p_pos; // �÷��̾�� npc �ٶ󺸴� ����
                            toNpc = Vector3_::Normalize(toNpc);



                            float PdotProduct = (npcLookAt.x * toNpc.x) + (npcLookAt.y * toNpc.y) + (npcLookAt.z * toNpc.z); // ����
                            float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

                            float PoutProduct = (toNpc.x * npcLookAt.z) - (toNpc.z * npcLookAt.x); // �տ� x ���� ���� ���� ����
                            if (PoutProduct > 0) // ����̸� npcLookAt�� toNpc�� ���� �ݽð�
                                radian *= -1.f;

                            float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
                            if (-0.03f > PdotProduct || 0.03f < PdotProduct) // ���ư����� ���� ������ �ȹٶ󺼶�
                            {
                                n.m_anim = A_WALK;
                                n.m_Mcondition = CON_IDLE;
                                if (g_clients[n.m_owner_id].m_boid[i].angle < n.m_total_angle) // ���� npc �������� ������ �����̼� ������ �� Ŭ��, ���̰� �÷���
                                {
                                    n.m_Rcondition = CON_LEFT;
                                    n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE); // ����ȸ��
                                }
                                else //���̰� ���̳ʽ�
                                {
                                    n.m_Rcondition = CON_RIGHT;
                                    n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE); // ������
                                }
                            }
                            else
                            {
                                _vec3 lastPos = *n_pos; // �ѹ��ڱ� ���ư��� ���� ������ ��ġ ���
                                n.m_Mcondition = CON_STRAIGHT;
                                n.m_Rcondition = CON_IDLE;
                                n.m_transform.BackWard(MOVE_TIME_ELAPSE); // �� ���ڱ� ������ ���ư�
                                 // ���� ���� �ٲ� ������ŭ ��Ż �ޱ� ���

                                _vec3 newLook = *n_pos - *p_pos;
                                newLook = Vector3_::Normalize(newLook);
                                _vec3 oldLook = lastPos - *p_pos;
                                oldLook = Vector3_::Normalize(oldLook);

                                PdotProduct = (newLook.x * oldLook.x) + (newLook.y * oldLook.y) + (newLook.z * oldLook.z); // ����
                                _vec3 PoutProduct;
                                PoutProduct.x = (oldLook.y * newLook.z) - (oldLook.z * newLook.y); // �ܰ�
                                PoutProduct.y = (oldLook.z * newLook.x) - (oldLook.x * newLook.z);
                                PoutProduct.z = (oldLook.x * newLook.y) - (oldLook.y * newLook.x);
                                float radian = acosf(PdotProduct); // ���� �̿��� ���� ����
                                if (PoutProduct.y < 0)
                                    radian *= -1.f;
                                float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����
                                n.m_total_angle += NPCangle;
                            }
                        }
                        else // �ڱⰡ �־���� ���� ��ġ�϶�
                        {
                            n.m_Mcondition = CON_IDLE;
                            _vec3 standard = -1.f * Vector3_::Normalize(*p_look);
                            _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look);

                            float PdotProduct = (npcLookAt.x * standard.x) + (npcLookAt.y * standard.y) + (npcLookAt.z * standard.z); // ����
                            float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

                            float PoutProduct = (standard.x * npcLookAt.z) - (standard.z * npcLookAt.x); // �տ� x ���� ���� ���� ����
                            if (PoutProduct > 0) // ����̸� npcLookAt�� standard�� ���� �ݽð�
                                radian *= -1.f;

                            float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����

                            if (NPCangle > 2.f || NPCangle < -2.f) // npc�� �ٶ󺸴� ������ �÷��̾�� ��ġ���� ������
                            {
                                n.m_anim = A_WALK;
                                if (NPCangle > 2.f)
                                {
                                    n.m_Rcondition = CON_LEFT;
                                    n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
                                }
                                else if (NPCangle < -2.f)
                                {
                                    n.m_Rcondition = CON_RIGHT;
                                    n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);
                                }
                            }
                            else
                            {
                                n.m_Rcondition = CON_IDLE;
                                n.m_anim = A_IDLE;
                            }
                        }
                    }
                }
            }
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

void Server::do_npc_rotate(int user_id, char con)
{
    for (int i = MY_NPC_START_SERVER(user_id); i <= MY_NPC_END_SERVER(user_id); ++i)
    {
        if (ST_ACTIVE == g_clients[i].m_status)
        {
            if (CON_LEFT == con)
                g_clients[i].m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
            else if (CON_RIGHT == con)
                g_clients[i].m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);

            //for (int player = 0; player < NPC_START; ++player)
            //{
            //    if (ST_ACTIVE != g_clients[player].m_status)
            //        continue;
            //    if (false == is_near(player, i)) // ��ó�� ���¾ָ� �������� ����
            //        continue;

            //    send_rotate_packet(player, i); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
            //}
        }
    }
}

float Server::dist_between(int user_id, int other_id)
{
    _vec3* pos1 = g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* pos2 = g_clients[other_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    float dist = sqrt((pos1->x - pos2->x) * (pos1->x - pos2->x) + (pos1->y - pos2->y) * (pos1->y - pos2->y)
        + (pos1->z - pos2->z) * (pos1->z - pos2->z));

    return dist;
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
            float hyp = sqrtf(Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);

            Dir = Dir / hyp; // ���Ⱑ ��ְ�
            Dir = Dir * 0.1f; // ��ְ� �������� ��ŭ ���� ���
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
    // �ϳ��� NPC�� �ൿ ���õ� �͵��� �ۼ��� ��

    // ���� ���� ���� ���� 2���� ���� ���ؼ� �񱳰� �Ͼ������ ���¸ӽ� Ÿ�� ����
    // Ŀ�ٶ� ���� ���� ���ϴ� if�� ������ switch ���¸ӽ� ������ switch ������
    // ���Ŵ� = ���� ���ְ� ���� ���� ���ϴ� if�� �ݱ�
    // func_id �� �ִ°� ���ο� ����, �װ� ���� ���¶� �� => Ŭ���ʿ��� ��� ��ü�� �������� ó��

    //bool keep_alive = false;
    //for (int i = 0; i < NPC_START; ++i) // ��� �÷��̾ ���ؼ�
    //{
    //    if (true == is_near(npc_id, i)) // �÷��̾� �þ߹��� �ȿ� �ְ�
    //    {
    //        if (ST_ACTIVE == g_clients[i].m_status) // �������ִ� �÷��̾��϶�
    //        {
    //            keep_alive = true; // ó�� �þ߹��� �ȿ� ���� �÷��̾� �������� npc�� Ȱ��ȭ
    //            break;
    //        }
    //    }
    //}
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
        //case FUNC_NPC_RANDMOVE:
        //{
        //    do_random_move(npc_id);
        //}
        //break;
        }
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
                if (n.m_LastMcondition != n.m_Mcondition)
                {
                    send_condition_packet(i, npc_id, CON_TYPE_MOVE);
                    //if (CON_IDLE == n.m_Mcondition)
                    //    cout << npc_id << " send move condition : CON_IDLE\n";
                    //else if (CON_STRAIGHT == n.m_Mcondition)
                    //    cout << npc_id << " send condition : CON_STRAIGHT\n";
                    //else if (CON_BACK == n.m_Mcondition)
                    //    cout << npc_id << " send condition : CON_BACK\n";
                }
                if (n.m_LastRcondition != n.m_Rcondition)
                {
                    send_condition_packet(i, npc_id, CON_TYPE_ROTATE);
                    //if (CON_IDLE == n.m_Rcondition)
                    //    cout << npc_id << " send rote condition : CON_IDLE\n";
                    //else if (CON_LEFT == n.m_Rcondition)
                    //    cout << npc_id << " send condition : CON_LEFT\n";
                    //else if (CON_RIGHT == n.m_Rcondition)
                    //    cout << npc_id << " send condition : CON_RIGHT\n";
                }
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
    if (n.m_LastMcondition != n.m_Mcondition)
        n.m_LastMcondition = n.m_Mcondition;
    if (n.m_LastRcondition != n.m_Rcondition)
        n.m_LastRcondition = n.m_Rcondition;
    if (n.m_LastAnim != n.m_anim)
        do_animation(npc_id, n.m_anim);

    if (ST_ACTIVE == g_clients[npc_id].m_status)
        add_timer(npc_id, g_clients[npc_id].m_last_order, FRAME_TIME); // ���� ���� �ݺ� ����
}

void Server::dead_reckoning(int player_id, ENUM_FUNCTION func_id)
{
    bool isMove = false;
    if (ST_ACTIVE == g_clients[player_id].m_status) // NPC�� ������ �÷��̾ Ȱ��ȭ �Ǿ� ������
    {
        switch (func_id)
        {
        case FUNC_PLAYER_STRAIGHT:
        {
            isMove = true;
            g_clients[player_id].m_last_move = FUNC_PLAYER_STRAIGHT;
            g_clients[player_id].m_transform.BackWard(MOVE_TIME_ELAPSE);
        }
        break;
        case FUNC_PLAYER_RUN:
        {
            isMove = true;
            g_clients[player_id].m_last_move = FUNC_PLAYER_RUN;
            g_clients[player_id].m_transform.BackWard(MOVE_TIME_ELAPSE * 2.f);
        }
        break;
        case FUNC_PLAYER_BACK:
        {
            isMove = true;
            g_clients[player_id].m_last_move = FUNC_PLAYER_BACK;
            g_clients[player_id].m_transform.Go_Straight(MOVE_TIME_ELAPSE);
        }
        break;
        case FUNC_PLAYER_LEFT:
        {
            g_clients[player_id].m_last_rotate = FUNC_PLAYER_LEFT;
            //do_npc_rotate(player_id, CON_LEFT);
            g_clients[player_id].m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
            g_clients[player_id].m_total_angle += (-ROTATE_TIME_ELAPSE * g_clients[player_id].m_rotate_speed) * 180.f / PIE;
        }
        break;
        case FUNC_PLAYER_RIGHT:
        {
            g_clients[player_id].m_last_rotate = FUNC_PLAYER_RIGHT;
            //do_npc_rotate(player_id, CON_RIGHT);
            g_clients[player_id].m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);
            g_clients[player_id].m_total_angle += ROTATE_TIME_ELAPSE * g_clients[player_id].m_rotate_speed * 180.f / PIE;
        }
        break;
        }
    }

    _vec3* newpos = g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    //_vec3 oldpos = *g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    //cout << newpos->x << " , " << newpos->z << endl;
    if (newpos->y < 0)
        newpos->y = 0;
    if (newpos->y >= (WORLD_HEIGHT - 1))
        newpos->y = WORLD_HEIGHT - 1;
    if (newpos->x < 0)
        newpos->x = 0;
    if (newpos->x >= (WORLD_HORIZONTAL - 1))
        newpos->x = WORLD_HORIZONTAL - 1;
    if (newpos->z >= (WORLD_VERTICAL - 1))
        newpos->z = WORLD_VERTICAL - 1;
    if (newpos->z < 0)
        newpos->z = 0;

    for (auto& c : g_clients) // aabb �浹üũ
    {
        if (c.second.m_id == player_id)
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (ST_DEAD == c.second.m_status)
            continue;
        if (false == is_near(c.second.m_id, player_id)) // ��ó�� ���¾ִ� �׳� �������� ����
            continue;
       /* if (check_basic_collision(player_id, c.second.m_id))
        {
            set_formation(c.second.m_id);
            break;
        }*/
        //if (check_obb_collision(user_id, c.second.m_id))
        //{

        //}

    }
    set_formation(player_id);

    g_clients[player_id].m_cLock.lock();
    unordered_set<int> old_viewlist = g_clients[player_id].m_view_list;
    // ���纻 �丮��Ʈ�� �ٸ� �����尡 �����ϸ� ��¼��? �� ������ �����ؾ���
    g_clients[player_id].m_cLock.unlock();
    unordered_set<int> new_viewlist;

    for (auto& c : g_clients)
    {
        if (c.second.m_id == player_id)
            continue;
        if (false == is_near(c.second.m_id, player_id)) // ��ó�� ���¾ִ� �׳� �������� ����
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        new_viewlist.insert(c.second.m_id); // �� �þ� �����ȿ� ������ �ٸ� ��ü���� ���̵� ����
    }

    for (auto new_vl : new_viewlist) // ������ ������ �þ� ������ ���Ͽ�
    {
        if (0 == old_viewlist.count(new_vl)) // ���� �丮��Ʈ�� new_vl�� ������ 0�� �϶� = ���� �þ߿� ���� ���϶�
        {
            send_enter_packet(player_id, new_vl); // �ٸ� ��ü���� ������ ������ ����

            if (false == is_player(new_vl)) // ���� �þ߿� ���� �ְ� �÷��̾ �ƴϸ� �� �ݺ��� �ѱ� / �� �ƴ϶� npc �ߵ����ְ� �ѱ�
            {
                continue;
            }

            g_clients[new_vl].m_cLock.lock();
            if (0 == g_clients[new_vl].m_view_list.count(player_id)) // ����� �丮��Ʈ�� ���� ���ٸ�
            {
                g_clients[new_vl].m_cLock.unlock();
                send_enter_packet(new_vl, player_id); // ���� ���� ������ �ٸ� ��ü�鿡�� ����
            }
            else
            {
                g_clients[new_vl].m_cLock.unlock();
                //send_move_packet(new_vl, player_id); // ���� ������ ������ �ٸ� ��ü�鿡�� ����
            }
        }
        else // �̵� �� �Ŀ� �� �þ߿� ���̴� �÷��̾��ε� �������� ������ �ִ� = ���� �þ߿� �ִ� ��
        {
            if (false == is_player(new_vl)) // npc���� �� ���ο� �������� ������ �ʿ䰡 ����
                continue;

            g_clients[new_vl].m_cLock.lock();
            if (0 != g_clients[new_vl].m_view_list.count(player_id))
            {
                g_clients[new_vl].m_cLock.unlock();
                //send_move_packet(new_vl, player_id);
            }
            else
            {
                g_clients[new_vl].m_cLock.unlock();
                send_enter_packet(new_vl, player_id);
            }
        }
    }

    for (auto old_vl : old_viewlist) // �����̱� �� �þ߹����� ���Ͽ�
    {
        if (0 == new_viewlist.count(old_vl)) // �� �þ߹����� old_vl ������ 0�϶� = �þ� �������� ��� ��ü�϶�
        {
            send_leave_packet(player_id, old_vl); // ������ ��� ��ü�� ������ �˸�

            if (false == is_player(old_vl)) // npc���� ���� �����ٴ°� �Ⱦ˷��� �ȴ�
                continue;

            g_clients[old_vl].m_cLock.lock();
            if (0 != g_clients[old_vl].m_view_list.count(player_id))
            {
                g_clients[old_vl].m_cLock.unlock();
                send_leave_packet(old_vl, player_id); // ��� ��ü���� ���� ������ �˸�
            }
            else // �Ǽ��ϱ� ����, else�� �� ������� unlock ���ٰ�, �ȱ׷��� ���� �Ҹ����� �� ��Ǯ��
            {
                g_clients[old_vl].m_cLock.unlock();
            }
        }
    }
    ;
    if (isMove)
    {
        if (g_clients[player_id].m_last_move == g_clients[player_id].m_curr_move)
        {
            add_timer(player_id, g_clients[player_id].m_last_move, FRAME_TIME); // ���� ���� �ݺ� ����
            return;
        }
    }
    else
    {
        if (g_clients[player_id].m_last_rotate == g_clients[player_id].m_curr_rotate)
        {
            add_timer(player_id, g_clients[player_id].m_last_rotate, FRAME_TIME); // ���� ���� �ݺ� ����
        }
    }
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

void Server::do_timer()
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
            case FUNC_BATTLE:
            case FUNC_CHECK_FLAG:
            case FUNC_CHECK_TIME:
            case FUNC_PLAYER_IDLE:
            case FUNC_PLAYER_STRAIGHT:
            case FUNC_PLAYER_BACK:
            case FUNC_PLAYER_LEFT:
            case FUNC_PLAYER_RIGHT:
            case FUNC_PLAYER_RUN:
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

void Server::send_condition_packet(int user_id, int other_id, unsigned char type)
{
    sc_packet_condition packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_CONDITION;
    packet.con_type = type;
    if (CON_TYPE_MOVE == type)
    {
        packet.condition = g_clients[other_id].m_Mcondition;
    }
    else if (CON_TYPE_ROTATE == type)
    {
        packet.condition = g_clients[other_id].m_Rcondition;
    }
    //packet.move_time = g_clients[mover].m_move_time; // ��Ʈ���� �׽�Ʈ
    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::send_fix_packet(int user_id, int other_id)
{
    sc_packet_fix packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_FIX;

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
    packet.p_z = pos._43;

    //packet.move_time = g_clients[mover].m_move_time; // ��Ʈ���� �׽�Ʈ
    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
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

            if (StartGame_PlayerCount < count)
            {
                //add_timer(-1, FUNC_CHECK_FLAG, 100);// ���� �÷��� �ð� ������ �Լ�
                //add_timer(-1, FUNC_CHECK_TIME, 1000);// ���� �÷��� �ð� ������ �Լ�
                isGameStart = true;
                cout << "Game Routine Start!\n";
                break;
            }
        }
    }
}

void Server::initialize_clients()
{
    for (int i = 0; i <= MAX_USER; ++i)
    {
        g_clients[i].m_id = i; // ���� ���
        g_clients[i].m_status = ST_FREE; // ����� ��Ƽ������ �ϱ����� �̱۽������϶� ����ϴ� �Լ�, �� ���ʿ�
    }
}

void Server::initialize_objects()
{
    for (int i = NPC_START; i < MAX_OBJECT; ++i)
    {
        g_clients[i].m_id = i; // ���� ���
        g_clients[i].m_status = ST_FREE; // ����� ��Ƽ������ �ϱ����� �̱۽������϶� ����ϴ� �Լ�, �� ���ʿ�
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
}

void Server::initialize_NPC(int player_id)
{
    for (int npc_id = MY_NPC_START_SERVER(player_id); npc_id <= MY_NPC_END_SERVER(player_id); npc_id++)
    {
        if (ST_ACTIVE != g_clients[npc_id].m_status)
        {
            cout << npc_id << " is intit\n";
            g_clients[npc_id].m_socket = 0;
            g_clients[npc_id].m_id = npc_id;
            g_clients[npc_id].m_owner_id = player_id;
            g_clients[npc_id].m_last_order = FUNC_NPC_FOLLOW;
            g_clients[npc_id].m_team = g_clients[player_id].m_team;
            sprintf_s(g_clients[npc_id].m_name, "NPC %d", npc_id);
            g_clients[npc_id].m_cLock.lock();
            g_clients[npc_id].m_status = ST_SLEEP;
            g_clients[npc_id].m_cLock.unlock();
            g_clients[npc_id].m_hp = 200;
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_UP,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_UP));
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_LOOK,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK));
            g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_RIGHT,
                g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_RIGHT));
            g_clients[player_id].m_transform.Scaling(SCALE.x, SCALE.y, SCALE.z);
            g_clients[npc_id].m_class = C_WORKER;
            update_speed(npc_id);
            g_clients[npc_id].m_rotate_speed = XMConvertToRadians(90.f);
            g_clients[npc_id].m_transform.SetUp_Speed(g_clients[npc_id].m_move_speed, g_clients[npc_id].m_rotate_speed);
            g_clients[npc_id].m_troop = T_INFT;
            g_clients[npc_id].m_Mcondition = CON_IDLE;
            g_clients[npc_id].m_Rcondition = CON_IDLE;
            g_clients[npc_id].m_LastMcondition = CON_IDLE;
            g_clients[npc_id].m_LastRcondition = CON_IDLE;
            g_clients[npc_id].m_col.col_range = { 20.f * SCALE.x,80.f * SCALE.y,20.f * SCALE.z };
            g_clients[npc_id].m_col.radius = 20.f * SCALE.x;
            g_clients[npc_id].m_isOut = false;
            g_clients[npc_id].m_isFormSet = true;
            g_clients[npc_id].m_attack_target = -1;
            g_clients[npc_id].m_isFighting = false;
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
    packet.con_move = g_clients[other_id].m_Mcondition;
    packet.con_rotate = g_clients[other_id].m_Rcondition;

    strcpy_s(packet.name, g_clients[other_id].m_name);

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::send_attacked_packet(int user_id, int other_id)
{
    sc_packet_attacked packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ATTACKED;
    packet.id = other_id;
    packet.hp = g_clients[other_id].m_hp;
    //cout << user_id << " saw " << other_id << " attacked\n";
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

void Server::send_chat_packet(int listen_id, int chatter_id, char mess[])
{
    sc_packet_chat packet;
    packet.id = chatter_id; // ä�� ������ �������
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_CHAT;
    strcpy_s(packet.message, mess);

    send_packet(listen_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
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
    //cout << idler << " do idle\n";
    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::do_attack(int npc_id)
{
    SESSION& n = g_clients[npc_id];
    _vec3 n_look = *n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    n_look = -1.f * Vector3_::Normalize(n_look);
    _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);

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
            // Ȱ��ȭ �����̰� �� ���� �ƴϰ� ���ݹ��� �ȿ� �ִ� ����϶�
            n.m_attack_target = i;
            //for (int i = 0; i < NPC_START; ++i) // npc �þ߹��� �� �ִ� �÷��̾�鿡�� ��ȣ ������ ��
            //{
            //    if (ST_ACTIVE != g_clients[i].m_status)
            //        continue;
            //    if (true == is_near(npc_id, i))
            //    {
            //        send_fix_packet(i, npc_id);
            //    }
            //}
            break;
        }

        if (n.m_attack_target >= 0) // �ֺ��� ���� ������ Ž�� ���߰� �ٷ� ����
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

            if (NPCangle > 1.5f || NPCangle < -1.5f) // npc�� �ٶ󺸴� ������ �÷��̾�� ��ġ���� ������
            {
                n.m_Mcondition = CON_STRAIGHT;
                n.m_transform.BackWard(MOVE_TIME_ELAPSE);
                n.m_anim = A_WALK;
                if (NPCangle > 1.f)
                {
                    n.m_Rcondition = CON_LEFT;
                    n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
                }
                else if (NPCangle < -1.f)
                {
                    n.m_Rcondition = CON_RIGHT;
                    n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);
                }
            }
            else // npc�� �ٶ󺸴� ������ �÷��̾�� ��ġ�Ҷ�
            {
                n.m_Mcondition = CON_STRAIGHT;
                n.m_Rcondition = CON_IDLE;
                n.m_anim = A_WALK;
                n.m_transform.BackWard(MOVE_TIME_ELAPSE);
            }
        }
    }
    else // ������ ����� ������ �Ǿ�����
    {
        _vec3 t_pos = *g_clients[n.m_attack_target].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
        _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
        _vec3 t_look = t_pos - n_pos;
        t_look = Vector3_::Normalize(t_look);

        float PdotProduct = (n_look.x * t_look.x) + (n_look.y * t_look.y) + (n_look.z * t_look.z); // ����
        float radian = acosf(PdotProduct); // ���� �̿��� ���� ����

        float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // �տ� x ���� ���� ���� ����
        if (PoutProduct > 0) // ����̸� n_look�� t_look�� ���� �ݽð�
            radian *= -1.f;

        float NPCangle = radian * 180.f / PIE; // ���� npc ��ġ�� �÷��̾� ���� � ���̳�����

        if (NPCangle > 1.5f || NPCangle < -1.5f) // npc�� ������ ����� �ȹٶ󺼶�
        {
            n.m_Mcondition = CON_IDLE;
            n.m_anim = A_WALK;
            if (NPCangle > 1.5f)
            {
                n.m_Rcondition = CON_LEFT;
                n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
            }
            else if (NPCangle < -1.5f)
            {
                n.m_Rcondition = CON_RIGHT;
                n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);
            }
        }
        else // npc�� ������ ����� �ٶ󺼶�
        {
            if (is_attackable(n.m_id, n.m_attack_target)) // ���� ���� �ȿ� ��������
            {
                if(!n.m_isFighting)
                    add_timer(npc_id, FUNC_BATTLE, 1);
                n.m_Mcondition = CON_IDLE;
                n.m_Rcondition = CON_IDLE;
                n.m_anim = A_ATTACK;
                n.m_isFighting = true;
            }
            else
            {
                n.m_isFighting = false;
                n.m_Mcondition = CON_STRAIGHT;
                n.m_transform.BackWard(MOVE_TIME_ELAPSE);
                n.m_Rcondition = CON_IDLE;
            }
        }
    }
}

void Server::do_dead(int id)
{
    //add_timer(id, FUNC_DEAD, 3000);
    for (int i = 0; i < NPC_START; ++i)
    {
        if (is_near(id, i)) // ������ �ֺ��� �ִ� �÷��̾��
        {
            send_dead_packet(i, id); // ���̰� ���� ü�� ������
            send_animation_packet(i, id, A_DEAD);
        }
    }
}

void Server::disconnect(int user_id)
{
    send_leave_packet(user_id, user_id); // �� �ڽ�
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
        (a_pos->z - b_pos->z)) < ATTACK_RADIUS)
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

void Server::do_battle(int id)
{
    SESSION& att = g_clients[id];
    g_clients[att.m_attack_target].m_hp -= ATTACK_DAMAGE;
    if (g_clients[att.m_attack_target].m_hp <= 0) // ���� ���¸�
    {
        if (ST_DEAD == g_clients[att.m_attack_target].m_status)
            return;
        g_clients[att.m_attack_target].m_hp = 0;
        //lock_guard <mutex> guardLock{ g_clients[att.m_attack_target].m_cLock };
        g_clients[att.m_attack_target].m_status = ST_DEAD;
        cout << att.m_attack_target << " is dead\n";
        send_leave_packet(att.m_attack_target, att.m_attack_target);
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (!is_near(i, att.m_attack_target))
                continue;
            // Ȱ��ȭ �Ǿ��ְ� ������ �þ߹��� �ȿ� �ִ� �����϶�
            send_leave_packet(i, att.m_attack_target); // ���� ü�� ��ε�ĳ����
        }
        att.m_attack_target = -1;
    }
    else // ���� ���Ŀ� ü���� �����ִ� ���¸�
    {
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (!is_near(i, att.m_attack_target))
                continue;
            // Ȱ��ȭ �Ǿ��ְ� ������ �þ߹��� �ȿ� �ִ� �����϶�
            send_attacked_packet(i, att.m_attack_target); // ���� ü�� ��ε�ĳ����
        }
        if (!is_player(id))
            add_timer(id, FUNC_BATTLE, 1000); // 1�ʵڿ� �� ����
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
                update_speed(user_id);
                g_clients[user_id].m_rotate_speed = XMConvertToRadians(90.f);
                g_clients[user_id].m_last_order = FUNC_END;
                g_clients[user_id].m_formation = F_SQUARE;
                g_clients[user_id].m_col.col_range = { 20.f * SCALE.x,80.f * SCALE.y,20.f * SCALE.z };
                g_clients[user_id].m_col.radius = 20.f * SCALE.x;
                g_clients[user_id].m_owner_id = user_id;
                g_clients[user_id].m_hp = 200;
                g_clients[user_id].m_total_angle = -90.f;
                g_clients[user_id].m_Mcondition = CON_IDLE;
                g_clients[user_id].m_Rcondition = CON_IDLE;
                g_clients[user_id].m_LastMcondition = CON_IDLE;
                g_clients[user_id].m_LastRcondition = CON_IDLE;
                g_clients[user_id].m_LastAnim = A_IDLE;
                g_clients[user_id].m_anim = A_IDLE;
                g_clients[user_id].m_troop = T_ALL;
                g_clients[user_id].m_owner_id = user_id; // ���� ���
                g_clients[user_id].m_view_list.clear(); // ���� �丮��Ʈ ������ ������ �ȵǴ� �ʱ�ȭ
                g_clients[user_id].m_isFighting = false;
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
        case FUNC_PLAYER_MOVE_FOR_NPC: // API_Send_message ȣ���
        {
            int npc_id = id;
            int player_id = overEx->player_id;
            event_player_move(player_id, npc_id);

            delete overEx;
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
        case FUNC_BATTLE:
            do_battle(id);
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
            if (play_time >= 0)
            {
                send_time_packet();
                play_time -= 1;
                add_timer(-1, FUNC_CHECK_TIME, 1000);
            }
            delete overEx;
            break;
        case FUNC_PLAYER_BACK:
            dead_reckoning(id, FUNC_PLAYER_BACK);
            delete overEx;
            break;
        case FUNC_PLAYER_STRAIGHT:
            dead_reckoning(id, FUNC_PLAYER_STRAIGHT);
            delete overEx;
            break;
        case FUNC_PLAYER_RUN:
            dead_reckoning(id, FUNC_PLAYER_RUN);
            delete overEx;
            break;
        case FUNC_PLAYER_LEFT:
            dead_reckoning(id, FUNC_PLAYER_LEFT);
            delete overEx;
            break;
        case FUNC_PLAYER_RIGHT:
            dead_reckoning(id, FUNC_PLAYER_RIGHT);
            delete overEx;
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

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // �׳� ���ε� ���� C++11�� �ִ� �����Լ��� ����
    listen(listenSocket, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // Ŀ�� ��ü ����, IOCP ��ü ����
    initialize_clients(); // Ŭ���̾�Ʈ ������ �ʱ�ȭ
    initialize_objects(); // ������Ʈ ������ �ʱ�ȭ
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
    for (int i = 0; i < 6; ++i) // ���⿡ �����ھ�� 4 �־��µ� ���� �ھ����ŭ �־ ����
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

bool Server::check_basic_collision(int a, int b)
{
    _vec3* a_pos = g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* b_pos = g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    float a_rad = g_clients[a].m_col.radius;
    float b_rad = g_clients[b].m_col.radius;
    //_vec3 a_col = g_clients[a].m_col.col_range;
    //_vec3 b_col = g_clients[b].m_col.col_range;

    //_vec3 a_min = { a_pos->x - a_col.x / 2,a_pos->y ,a_pos->z - a_col.z / 2 }; // AABB �浹
    //_vec3 a_max = { a_pos->x + a_col.x / 2,a_pos->y + a_col.y ,a_pos->z + a_col.z / 2 };
    //_vec3 b_min = { b_pos->x - b_col.x / 2,b_pos->y ,b_pos->z - b_col.z / 2 };
    //_vec3 b_max = { b_pos->x + b_col.x / 2,b_pos->y + b_col.y ,b_pos->z + b_col.z / 2 };

    //if ((a_min.x <= b_max.x && a_max.x >= b_min.x) &&
    //    (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
    //    (a_min.z <= b_max.z && a_max.z >= b_min.z))
    //{
    //    //Pos = PrevPos; ���� ��ġ�� �ǵ�����
    //    cout << "id " << a << " has collide with " << b << "\n";
    //    return true;
    //}
    //else
    //    return false;

    float dist = sqrt((a_pos->x - b_pos->x) * (a_pos->x - b_pos->x) +
        (a_pos->y - b_pos->y) * (a_pos->y - b_pos->y) +
        (a_pos->z - b_pos->z) * (a_pos->z - b_pos->z));

    float overlap = (a_rad + b_rad - dist);

    if (dist > 0 && dist < (a_rad + b_rad))
    {
        //cout << "id " << a << " has collide with " << b << "\n";
        //a_pos->x -= overlap * (a_pos->x - b_pos->x) / dist;
        //a_pos->y -= overlap * (a_pos->y - b_pos->y) / dist;
        //a_pos->z -= overlap * (a_pos->z - b_pos->z) / dist;
        if ((g_clients[a].m_owner_id == g_clients[b].m_owner_id) && a < b)
        {
            // ����
        }
        else
        {
            b_pos->x -= overlap * (a_pos->x - b_pos->x) / dist;
            b_pos->y -= overlap * (a_pos->y - b_pos->y) / dist;
            b_pos->z -= overlap * (a_pos->z - b_pos->z) / dist;

        }
        return true;
    }
    else
        return false;
}

//bool Server::check_obb_collision(int a, int b)
//{
//    _vec3 a_Points[4];
//    _vec3 b_Points[4];
//
//    _vec3 a_pos = *g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
//    _vec3 b_pos = *g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
//
//    CTransform set_a_pos;
//    set_a_pos.Set_StateInfo(CTransform::STATE_POSITION, &a_pos);
//
//    set_a_pos.Go_Left(g_clients[a].m_col.col_range.x / 2);
//    set_a_pos.BackWard(g_clients[a].m_col.col_range.z / 2);
//    a_Points[0] = *set_a_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    set_a_pos.Go_Right(g_clients[a].m_col.col_range.x / 2);
//    set_a_pos.BackWard(g_clients[a].m_col.col_range.z / 2);
//    a_Points[1] = *set_a_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    set_a_pos.Go_Left(g_clients[a].m_col.col_range.x / 2);
//    set_a_pos.Go_Straight(g_clients[a].m_col.col_range.z / 2);
//    a_Points[2] = *set_a_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    set_a_pos.Go_Right(g_clients[a].m_col.col_range.x / 2);
//    set_a_pos.Go_Straight(g_clients[a].m_col.col_range.z / 2);
//    a_Points[3] = *set_a_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    CTransform set_b_pos;
//    set_b_pos.Set_StateInfo(CTransform::STATE_POSITION, &b_pos);
//
//    set_b_pos.Go_Left(g_clients[b].m_col.col_range.x / 2);
//    set_b_pos.BackWard(g_clients[b].m_col.col_range.z / 2);
//    b_Points[0] = *set_b_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    set_b_pos.Go_Right(g_clients[b].m_col.col_range.x / 2);
//    set_b_pos.BackWard(g_clients[b].m_col.col_range.z / 2);
//    b_Points[1] = *set_b_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    set_b_pos.Go_Left(g_clients[b].m_col.col_range.x / 2);
//    set_b_pos.Go_Straight(g_clients[b].m_col.col_range.z / 2);
//    b_Points[2] = *set_b_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    set_b_pos.Go_Right(g_clients[b].m_col.col_range.x / 2);
//    set_b_pos.Go_Straight(g_clients[b].m_col.col_range.z / 2);
//    b_Points[3] = *set_b_pos.Get_StateInfo(CTransform::STATE_POSITION);
//
//    float a_rad = g_clients[a].m_col.radius;
//    float b_rad = g_clients[b].m_col.radius;
//    //_vec3 a_col = g_clients[a].m_col.col_range;
//    //_vec3 b_col = g_clients[b].m_col.col_range;
//
//    //_vec3 a_min = { a_pos->x - a_col.x / 2,a_pos->y ,a_pos->z - a_col.z / 2 };
//    //_vec3 a_max = { a_pos->x + a_col.x / 2,a_pos->y + a_col.y ,a_pos->z + a_col.z / 2 };
//    //_vec3 b_min = { b_pos->x - b_col.x / 2,b_pos->y ,b_pos->z - b_col.z / 2 };
//    //_vec3 b_max = { b_pos->x + b_col.x / 2,b_pos->y + b_col.y ,b_pos->z + b_col.z / 2 };
//
//    //if ((a_min.x <= b_max.x && a_max.x >= b_min.x) &&
//    //    (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
//    //    (a_min.z <= b_max.z && a_max.z >= b_min.z))
//    //{
//    //    //Pos = PrevPos; ���� ��ġ�� �ǵ�����
//    //    cout << "id " << a << " has collide with " << b << "\n";
//    //    return true;
//    //}
//    //else
//    //    return false;
//
//    float dist = sqrt((a_pos->x - b_pos->x) * (a_pos->x - b_pos->x) +
//        (a_pos->y - b_pos->y) * (a_pos->y - b_pos->y) +
//        (a_pos->z - b_pos->z) * (a_pos->z - b_pos->z));
//
//    float overlap = (a_rad + b_rad - dist);
//
//   
//        return true;
//
//    else
//        return false;
//}

void Server::do_push(int pusher, int target)
{
    _vec3* a_pos = g_clients[pusher].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* b_pos = g_clients[target].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    float a_rad = g_clients[pusher].m_col.radius;
    float b_rad = g_clients[target].m_col.radius;

    float dist = sqrt((a_pos->x - b_pos->x) * (a_pos->x - b_pos->x) +
        (a_pos->y - b_pos->y) * (a_pos->y - b_pos->y) +
        (a_pos->z - b_pos->z) * (a_pos->z - b_pos->z));

    if (dist > 0)
    {
        b_pos->x -= (a_pos->x - b_pos->x) / dist;
        b_pos->y -= (a_pos->y - b_pos->y) / dist;
        b_pos->z -= (a_pos->z - b_pos->z) / dist;

        do_move(target, GO_COLLIDE);
    }
}

bool Server::is_object(int id)
{
    if (id >= OBJECT_START)
        return true;
    else
        return false;
}