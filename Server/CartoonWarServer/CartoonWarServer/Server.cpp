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
            do_move(user_id, packet->con);
        else if (CON_TYPE_ROTATE == packet->con_type)
            do_rotate(user_id, packet->con);

        cout << user_id << "send condition" << packet->con << endl;
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
        do_move(user_id, GO_COLLIDE);
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


    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::send_move_fix_packet(int user_id, int other_id)
{
    sc_packet_move_fix packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE_FIX;
    packet.id = other_id;
    _vec3* pos = g_clients[other_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    packet.p_x = pos->x;
    packet.p_z = pos->z;

    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void Server::send_rotate_fix_packet(int user_id, int other_id)
{
    sc_packet_rotate_fix packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ROTATE_FIX;
    packet.id = other_id;
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
    g_clients[user_id].m_transform.SetUp_Speed(MOVE_SPEED_PLAYER, XMConvertToRadians(90.f));

    if (CON_IDLE == con)
    {
        g_clients[user_id].m_cLock.lock();
        g_clients[user_id].m_curr_rotate = FUNC_PLAYER_IDLE;
        g_clients[user_id].m_cLock.unlock();
        g_clients[user_id].m_Rcondition = CON_IDLE;
    }
    else if (CON_RIGHT == con)
    {
        g_clients[user_id].m_curr_rotate = FUNC_PLAYER_RIGHT;
        g_clients[user_id].m_Rcondition = CON_RIGHT;
        //g_clients[user_id].m_transform.Rotation_Y(ROTATE_SPEED);
        add_timer(user_id, FUNC_PLAYER_RIGHT, FRAME_TIME);
    }
    else if (CON_LEFT == con)
    {
        g_clients[user_id].m_curr_rotate = FUNC_PLAYER_LEFT;
        g_clients[user_id].m_Rcondition = CON_LEFT;
        //g_clients[user_id].m_transform.Rotation_Y(-ROTATE_SPEED);
        add_timer(user_id, FUNC_PLAYER_LEFT, FRAME_TIME);
    }

    g_clients[user_id].m_cLock.lock();
    unordered_set<int> copy_viewlist = g_clients[user_id].m_view_list;
    // ���纻 �丮��Ʈ�� �ٸ� �����尡 �����ϸ� ��¼��? �� ������ �����ؾ���
    g_clients[user_id].m_cLock.unlock();

    send_condition_packet(user_id, user_id, CON_TYPE_ROTATE); // ���� ���ư��ٴ� ���� ������, �ڿ��� ������ ���ư���, �ϴ� ���� �� ���ư��� �˸�
    if (CON_IDLE == con)
        send_rotate_fix_packet(user_id, user_id);
    for (auto cpy_vl : copy_viewlist) // ������ ������ �þ� ������ ���Ͽ�
    {
        send_condition_packet(cpy_vl, user_id, CON_TYPE_ROTATE); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
        // �þ� ���� ó���� move ���ؼ��� �ϰ� ȸ���� ������ �ְ������ �ȴ�
        if (CON_IDLE == con)
            send_rotate_fix_packet(cpy_vl, user_id);
    }
    do_npc_rotate(user_id, con);
}

void Server::do_move(int user_id, char con)
{
    g_clients[user_id].m_transform.SetUp_Speed(MOVE_SPEED_PLAYER, XMConvertToRadians(90.f));
    //_vec3* newpos = g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    //_vec3 oldpos = *g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    switch (con)
    {
    case CON_IDLE:
    {
        g_clients[user_id].m_cLock.lock();
        g_clients[user_id].m_curr_move = FUNC_PLAYER_IDLE;
        g_clients[user_id].m_cLock.unlock();
        g_clients[user_id].m_Mcondition = CON_IDLE;
    }
    break;
    case CON_STRAIGHT:
        //if (newpos->z >= 0 || newpos->x >= 0)
    {
        g_clients[user_id].m_curr_move = FUNC_PLAYER_STRAIGHT;
        g_clients[user_id].m_Mcondition = CON_STRAIGHT;
        //g_clients[user_id].m_transform.BackWard(MOVE_SPEED_PLAYER);
        add_timer(user_id, FUNC_PLAYER_STRAIGHT, FRAME_TIME);
    }
    break;
    case CON_RUN:
        //if (newpos->z >= 0 || newpos->x >= 0)
    {
        g_clients[user_id].m_curr_move = FUNC_PLAYER_RUN;
        g_clients[user_id].m_Mcondition = CON_RUN;
        //g_clients[user_id].m_transform.BackWard(MOVE_SPEED_PLAYER * 2.f);
        add_timer(user_id, FUNC_PLAYER_RUN, FRAME_TIME);
    }
    break;
    case CON_BACK:
        //if (newpos->z < WORLD_VERTICAL || newpos->x < WORLD_HORIZONTAL)
    {
        g_clients[user_id].m_curr_move = FUNC_PLAYER_BACK;
        g_clients[user_id].m_Mcondition = CON_BACK;
        //g_clients[user_id].m_transform.Go_Straight(MOVE_SPEED_PLAYER);
        add_timer(user_id, FUNC_PLAYER_BACK, FRAME_TIME);
    }
    break;
    default:
        cout << "Unknown Direction From cs_move_packet !\n";
        DebugBreak();
        exit(-1);
    }

    g_clients[user_id].m_cLock.lock();
    unordered_set<int> copy_viewlist = g_clients[user_id].m_view_list;
    // ���纻 �丮��Ʈ�� �ٸ� �����尡 �����ϸ� ��¼��? �� ������ �����ؾ���
    g_clients[user_id].m_cLock.unlock();

    send_condition_packet(user_id, user_id, CON_TYPE_MOVE); // ���� ���ư��ٴ� ���� ������, �ڿ��� ������ ���ư���, �ϴ� ���� �� ���ư��� �˸�
    if (CON_IDLE == con)
        send_move_fix_packet(user_id, user_id);
    for (auto cpy_vl : copy_viewlist) // ������ ������ �þ� ������ ���Ͽ�
    {
        send_condition_packet(cpy_vl, user_id, CON_TYPE_MOVE); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
        if (CON_IDLE == con)
            send_move_fix_packet(cpy_vl, user_id);
        // �þ� ���� ó���� move ���ؼ��� �ϰ� ȸ���� ������ �ְ������ �ȴ�
    }
}



void Server::set_formation(int user_id)
{
    SESSION& c = g_clients[user_id];
    _matrix playerMat = c.m_transform.Get_Matrix();
    _vec3 playerLookAt = *c.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    playerLookAt = Vector3_::Normalize(playerLookAt);
    _vec3 playerPos = *c.m_transform.Get_StateInfo(CTransform::STATE_POSITION);

    _vec3 standard = { 0.f,0.f,1.f };
    _vec3 set_pos = {};

    float PdotProduct = (playerLookAt.x * standard.x) + (playerLookAt.y * standard.y) + (playerLookAt.z * standard.z);
    float radian = acosf(PdotProduct); // �÷��̾ �ٶ󺸴� ����� 0,0,1 ���� ������ ����
    float angle = radian * 180.f / PIE; // 0,0,1z �� �÷��̾� look ������ ����
    float radius = FORMATION_SPACE; // npc �÷��̾� ����
    
    switch (c.m_formation)
    {
    case FM_FLOCK:
    {
        if (1 == c.m_boid.size())
        {
            set_pos.x = radius * cosf((angle + 90.f) * (PIE / 180.f));
            set_pos.z = radius * sinf((angle + 90.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0]->m_target_pos = new_pos;
        }
        else if (2 == c.m_boid.size())
        {
            set_pos.x = radius * cosf((angle + 90.f) * (PIE / 180.f));
            set_pos.z = radius * sinf((angle + 90.f) * (PIE / 180.f));
            _vec3 new_pos = playerPos + set_pos;
            c.m_boid[0]->m_target_pos = new_pos;

            set_pos = {};
            set_pos.x = radius * cosf((angle - 90.f) * (PIE / 180.f));
            set_pos.z = radius * sinf((angle - 90.f) * (PIE / 180.f));
            _vec3 new_pos1 = playerPos + set_pos;
            c.m_boid[1]->m_target_pos = new_pos1;
        }
      /*  else if (3 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Go_Right(FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;
        }
        else if (4 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Go_Left(FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;

            set_pos.Go_Right(FORMATION_SPACE);
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
            set_pos.Go_Left(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos;
        }
        else if (2 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;
        }
        else if (3 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            set_pos.Go_Straight(FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;
        }
        else if (4 == c.m_boid.size())
        {
            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos1 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[0]->m_target_pos = *new_pos1;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(FORMATION_SPACE);
            set_pos.BackWard(FORMATION_SPACE);
            _vec3* new_pos2 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[1]->m_target_pos = *new_pos2;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Left(FORMATION_SPACE);
            set_pos.Go_Straight(FORMATION_SPACE);
            _vec3* new_pos3 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[2]->m_target_pos = *new_pos3;

            set_pos.Set_Matrix(&temp);
            set_pos.Go_Right(FORMATION_SPACE);
            set_pos.Go_Straight(FORMATION_SPACE);
            _vec3* new_pos4 = set_pos.Get_StateInfo(CTransform::STATE_POSITION);
            c.m_boid[3]->m_target_pos = *new_pos4;
        }*/
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
    for (int i = 0; i < g_clients[g_clients[npc_id].m_owner_id].m_boid.size(); ++i)
    {
        if (g_clients[g_clients[npc_id].m_owner_id].m_boid[i]->m_id == g_clients[npc_id].m_id)
        {
            _vec3 n_pos = *g_clients[npc_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            if (n_pos != g_clients[g_clients[npc_id].m_owner_id].m_boid[i]->m_target_pos)
            {
                _vec3 Dir = move_to_spot(npc_id, &g_clients[g_clients[npc_id].m_owner_id].m_boid[i]->m_target_pos);// �� ����� ���� ��ġ���� �׳� �����̵�
                //g_clients[g_clients[npc_id].m_owner_id].m_boid[i]->m_target_pos �̰� ���� ��ġ��
                //_vec3 standard = { 0.f,0.f,1.f };
                _vec3 *pos = g_clients[npc_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);

                //_vec3* playerLookAt = g_clients[g_clients[npc_id].m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
                //float PdotProduct = (playerLookAt->x * standard.x) + (playerLookAt->y * standard.y) + (playerLookAt->z * standard.z);
                //float radian = acosf(PdotProduct); // �÷��̾ �ٶ󺸴� ����� 0,0,1 ���� ������ ����
                //float radius = sqrt((n_pos.x - p_pos.x) * (n_pos.x - p_pos.x) + (n_pos.y - p_pos.y) * (n_pos.y - p_pos.y)
                //    + (n_pos.z - p_pos.z) * (n_pos.z - p_pos.z)); // �÷��̾�� npc ���� �Ÿ� = ������

                //_vec3* npcLookAt = g_clients[npc_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
                //float NdotProduct = (npcLookAt->x * standard.x) + (npcLookAt->y * standard.y) + (npcLookAt->z * standard.z);


                //float lookSize = sqrt((playerLookAt->x * playerLookAt->x) + (playerLookAt->z * playerLookAt->z));
                //float stanSize = sqrt((standard.x * standard.x) + (standard.z * standard.z));
                _vec3 new_pos = *pos + Dir;
                if (*pos != new_pos)
                {
                    g_clients[npc_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &new_pos);

                    for (int i = 0; i < OBJECT_START; ++i)
                    {
                        if (ST_ACTIVE != g_clients[i].m_status)
                            continue;
                        if (!is_near(i, npc_id))
                            continue;
                        if (FUNC_DEAD == g_clients[i].m_last_order)
                            continue;
                        if (check_basic_collision(npc_id, i)) // Ȱ��ȭ �Ǿ��ְ� �þ߹��� ���� �÷��̾�+npc�� ���ؼ�
                            do_move(i, GO_COLLIDE);
                        //if (check_obb_collision(user_id, c.second.m_id))
    //{

    //}
                    }

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
                    if (g_clients[npc_id].m_anim != A_WALK)
                        do_animation(npc_id, A_WALK);
                    break;
                }
                else //if (*pos == new_pos && !isOnce)
                {
                    if (g_clients[npc_id].m_anim != A_IDLE)
                        do_animation(npc_id, A_IDLE);
                }
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

void Server::do_npc_rotate(int user_id, char con)
{
    for (int i = MY_NPC_START(user_id); i <= MY_NPC_END(user_id); ++i)
    {
        if (ST_ACTIVE == g_clients[i].m_status)
        {
            if (CON_LEFT == con)
                g_clients[i].m_transform.Rotation_Y(-MOVE_SPEED_NPC);
            else if (CON_RIGHT == con)
                g_clients[i].m_transform.Rotation_Y(MOVE_SPEED_NPC);

            for (int player = 0; player < NPC_START; ++player)
            {
                if (ST_ACTIVE != g_clients[player].m_status)
                    continue;
                if (false == is_near(player, i)) // ��ó�� ���¾ָ� �������� ����
                    continue;

                //send_rotate_packet(player, i); // �� �þ߹��� �ȿ� �ִ� �ֵ����׸� ���� ���ư��ٴ°� ����
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

            Dir = Dir / hyp; // ���Ⱑ ��ְ�
            Dir = Dir * MOVE_SPEED_NPC; // ��ְ� �������� ��ŭ ���� ���
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

    if (ST_ACTIVE == g_clients[g_clients[npc_id].m_owner_id].m_status) // NPC�� ������ �÷��̾ Ȱ��ȭ �Ǿ� ������
    {
        if (FUNC_DEAD == g_clients[npc_id].m_last_order)
            return;

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
        add_timer(npc_id, g_clients[npc_id].m_last_order, 1000); // ���� ���� �ݺ� ����
    else
        add_timer(npc_id, g_clients[npc_id].m_last_order, FRAME_TIME); // ���� ���� �ݺ� ����
}

void Server::dead_reckoning(int player_id, ENUM_FUNCTION func_id)
{
    if (ST_ACTIVE == g_clients[player_id].m_status) // NPC�� ������ �÷��̾ Ȱ��ȭ �Ǿ� ������
    {
        if (FUNC_DEAD == g_clients[player_id].m_last_order)
            return;



        switch (func_id)
        {
        case FUNC_PLAYER_STRAIGHT:
        {
            g_clients[player_id].m_last_move = FUNC_PLAYER_STRAIGHT;
            g_clients[player_id].m_transform.BackWard(MOVE_TIME_ELAPSE);
        }
        break;
        case FUNC_PLAYER_RUN:
        {
            g_clients[player_id].m_last_move = FUNC_PLAYER_RUN;
            g_clients[player_id].m_transform.BackWard(MOVE_TIME_ELAPSE * 2.f);
        }
        break;
        case FUNC_PLAYER_BACK:
        {
            g_clients[player_id].m_last_move = FUNC_PLAYER_BACK;
            g_clients[player_id].m_transform.Go_Straight(MOVE_TIME_ELAPSE);
        }
        break;
        case FUNC_PLAYER_LEFT:
        {
            g_clients[player_id].m_last_rotate = FUNC_PLAYER_LEFT;
            g_clients[player_id].m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE);
        }
        break;
        case FUNC_PLAYER_RIGHT:
        {
            g_clients[player_id].m_last_rotate = FUNC_PLAYER_RIGHT;
            g_clients[player_id].m_transform.Rotation_Y(ROTATE_TIME_ELAPSE);
        }
        break;
        }
    }

    _vec3* newpos = g_clients[player_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    //_vec3 oldpos = *g_clients[user_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    cout << newpos->x << " , " << newpos->z << endl;
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
        if (FUNC_DEAD == c.second.m_last_order)
            continue;
        if (false == is_near(c.second.m_id, player_id)) // ��ó�� ���¾ִ� �׳� �������� ����
            continue;
        if (check_basic_collision(player_id, c.second.m_id))
        {
            set_formation(c.second.m_id);
            break;
        }
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
        if (false == is_near(c.second.m_id, player_id)) // ��ó�� ���¾ִ� �׳� �������� ����
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (c.second.m_id == player_id)
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

    g_clients[player_id].m_cLock.lock();
    if (g_clients[player_id].m_last_move == g_clients[player_id].m_curr_move)
    {
        g_clients[player_id].m_cLock.unlock();
        add_timer(player_id, g_clients[player_id].m_last_move, FRAME_TIME); // ���� ���� �ݺ� ����
    }
    else
        g_clients[player_id].m_cLock.unlock();
    g_clients[player_id].m_cLock.lock();
    if (g_clients[player_id].m_last_rotate == g_clients[player_id].m_curr_rotate)
    {
        g_clients[player_id].m_cLock.unlock();
        add_timer(player_id, g_clients[player_id].m_last_rotate, FRAME_TIME); // ���� ���� �ݺ� ����
    }
    else
        g_clients[player_id].m_cLock.unlock();
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
            case FUNC_NPC_RANDMOVE:
            case FUNC_NPC_ATTACK:
            case FUNC_NPC_DEFENCE:
            case FUNC_NPC_HOLD:
            case FUNC_DEAD:
            case FUNC_NPC_FOLLOW:
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
                //random_move_npc(event.obj_id);
                //add_timer(event.obj_id, (ENUM_FUNCTION)event.event_id, 1000);
                // Ÿ�̸� �����忡�� �����̴°� ó������ �� �ϸ� ����ȭ�� ���ϴ�
                // PostQueuedCompletionStatus �̰ɷ� worket thread�� �۾� �Ѱ��ְ� ���⼱ � �̺�Ʈ������ �˷���
                // ������ ����ü ���� �ʱ�ȭ �����൵ �Ǵ°� PostQueuedCompletionStatus ��ü�� ��¥ �־��ִ°� �״�� GetQueued�� �Ѱ��༭ ������
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

void Server::enter_game(int user_id, char name[])
{
    g_clients[user_id].m_cLock.lock(); // name, m_status ��
    strcpy_s(g_clients[user_id].m_name, name);
    g_clients[user_id].m_name[MAX_ID_LEN] = NULL; // �������� NULL �־��ִ� ó��
    send_login_ok_packet(user_id); // ���� ������ �÷��̾� �ʱ�ȭ ���� ������
    for (int i = 0; i < 5; ++i)
        send_flag_info_packet(i, user_id); // ���� ������ �÷��̾� �ʱ�ȭ ���� ������
    g_clients[user_id].m_status = ST_ACTIVE; // �ٸ� Ŭ������� ���� ���� ������ �������� ST_ACTIVE�� �ٲ��ֱ�
    g_clients[user_id].m_cLock.unlock();
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
            g_clients[player_id].m_transform.Scaling(SCALE.x, SCALE.y, SCALE.z);
            g_clients[npc_id].m_speed = MOVE_SPEED_NPC;
            g_clients[npc_id].m_class = CLASS::CLASS_WORKER;
            g_clients[npc_id].m_Mcondition = CON_IDLE;
            g_clients[npc_id].m_Rcondition = CON_IDLE;
            g_clients[npc_id].m_col.col_range = { 20.f * SCALE.x,80.f * SCALE.y,20.f * SCALE.z };
            g_clients[npc_id].m_col.radius = 20.f * SCALE.x;
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
            break;
        }
        else
        {
            continue; // ���� �����Ұ� (�ӽù���), �÷��̾��� ��� npc active�϶� ���̻� �߰� �ȵǰ� ó��
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
    packet.con_move = g_clients[other_id].m_Mcondition;
    packet.con_rotate = g_clients[other_id].m_Rcondition;

    strcpy_s(packet.name, g_clients[other_id].m_name);
    packet.o_type = O_HUMAN; // �ٸ� �÷��̾���� ���� ����

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

void Server::send_dead_packet(int user_id, int other_id)
{
    sc_packet_dead packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_DEAD;
    packet.id = g_clients[other_id].m_id;
    //cout << user_id << " saw " << other_id << " dead\n";
    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
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

void Server::send_npc_add_ok_packet(int user_id, int other_id)
{
    sc_packet_enter packet;
    packet.id = other_id; // �߰��� npc ���̵�
    packet.hp = g_clients[other_id].m_hp;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ADD_NPC_OK;

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void Server::do_animation(int user_id, unsigned char anim)
{
    g_clients[user_id].m_anim = anim;
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

void Server::do_attack(int user_id)
{
    //cout << user_id << "is do attack\n";
    for (auto& c : g_clients)
    {
        if (ST_ACTIVE != c.second.m_status) // ������ ������ �ֵ� ����
            continue;
        if (is_object(c.second.m_id))
            continue;
        if (false == is_near(c.second.m_id, user_id)) // ��ó�� ���¾ֵ� ����
            continue;
        if (c.second.m_id == user_id) // �� �ڽ� ����
            continue;
        if (c.second.m_owner_id == user_id) // �� npc�� ����
            continue;
        if (true == is_attackable(c.second.m_id, user_id)) // �� ���� ���� �ȿ� ��� + ��npc �� ������
        {
            c.second.m_hp -= ATTACK_DAMAGE;
            if (0 < c.second.m_hp) // �°� �ǰ� ����������
            {
                do_push(user_id, c.second.m_id);
                for (int i = 0; i < NPC_START; ++i)
                {
                    if (is_near(c.second.m_id, i)) // ������ �ֺ��� �ִ� �÷��̾��
                    {
                        send_attacked_packet(i, c.second.m_id);
                        send_animation_packet(i, c.second.m_id, A_HIT);
                    }
                }
            }
            else // ������
            {
                c.second.m_hp = 0;
                g_clients[c.second.m_id].m_last_order = FUNC_DEAD;
                if (is_player(c.second.m_id)) // ���� �ְ� ������
                {
                    send_dead_packet(c.second.m_id, c.second.m_id); // �ڱ� �ڽſ��� ����
                    send_animation_packet(c.second.m_id, c.second.m_id, A_DEAD);
                    do_dead(c.second.m_id);
                    for (int npc_id = MY_NPC_START(c.second.m_id); npc_id <= MY_NPC_END(c.second.m_id); npc_id++)
                    {
                        if (ST_ACTIVE == g_clients[npc_id].m_status)
                        {
                            do_dead(npc_id);
                        }
                    }
                }
                else // ���� �ְ� npc ��
                {
                    do_dead(c.second.m_id);
                }
            }
        }
    }
}

void Server::do_dead(int id)
{
    add_timer(id, FUNC_DEAD, 3000);
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
            send_leave_packet(g_clients[i].m_id, user_id); // ������ send_leave_packet ���ο��� �丮��Ʈ ���� ���༭ ���⿡ ���� �� �ʿ�X
        }
        // c.second.m_cLock.unlock();
    }
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
                g_clients[user_id].m_class = CLASS::CLASS_WORKER;
                g_clients[user_id].m_col.col_range = { 20.f * SCALE.x,80.f * SCALE.y,20.f * SCALE.z };
                g_clients[user_id].m_col.radius = 20.f * SCALE.x;
                g_clients[user_id].m_speed = MOVE_SPEED_PLAYER;
                g_clients[user_id].m_hp = 100;
                g_clients[user_id].m_Mcondition = CON_IDLE;
                g_clients[user_id].m_Rcondition = CON_IDLE;
                g_clients[user_id].m_owner_id = user_id; // ���� ���
                if (0 == user_id)
                {
                    g_clients[user_id].m_team = TEAM_RED;
                    _vec3 pos = { 50.f, 0.f, 70.f };
                    g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
                }
                else
                {
                    g_clients[user_id].m_team = TEAM_BLUE;
                    _vec3 pos = { 70.f, 0.f, 70.f };
                    g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
                }

                //{ // ��Ʈ���� �׽�Ʈ
                //    _vec3 pos = { (float)(rand()%WORLD_HORIZONTAL), 0.f, (float)(rand() % WORLD_HORIZONTAL) };
                //    g_clients[user_id].m_transform.Set_StateInfo(CTransform::STATE_POSITION, &pos);
                //}
                g_clients[user_id].m_last_order = FUNC_END;
                g_clients[user_id].m_formation = FM_FLOCK;
                g_clients[user_id].m_view_list.clear(); // ���� �丮��Ʈ ������ ������ �ȵǴ� �ʱ�ȭ

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
        case FUNC_NPC_ATTACK: // API_Send_message ȣ���
            finite_state_machine(id, FUNC_NPC_ATTACK);
            delete overEx;
            break;
        case FUNC_NPC_DEFENCE: // API_Send_message ȣ���
            finite_state_machine(id, FUNC_NPC_DEFENCE);
            delete overEx;
            break;
        case FUNC_NPC_HOLD: // API_Send_message ȣ���
            finite_state_machine(id, FUNC_NPC_HOLD);
            delete overEx;
            break;
        case FUNC_NPC_FOLLOW: // API_Send_message ȣ���
            finite_state_machine(id, FUNC_NPC_FOLLOW);
            delete overEx;
            break;
        case FUNC_NPC_RANDMOVE:
            finite_state_machine(id, FUNC_NPC_RANDMOVE);
            delete overEx;
            break;
        case FUNC_DEAD:
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

        b_pos->x -= overlap * (a_pos->x - b_pos->x) / dist;
        b_pos->y -= overlap * (a_pos->y - b_pos->y) / dist;
        b_pos->z -= overlap * (a_pos->z - b_pos->z) / dist;

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

void Server::do_push(int pusher,int target)
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