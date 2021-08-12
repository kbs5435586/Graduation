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
    case CS_PACKET_CONDITION:
    {
        cs_packet_condition* packet = reinterpret_cast<cs_packet_condition*>(buf);
        // g_clients[user_id].m_move_time = packet->move_time; // 스트레스 테스트
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
    // 복사본 뷰리스트에 다른 쓰레드가 접근하면 어쩌냐? 그 정도는 감수해야함
    c.m_cLock.unlock();

    send_condition_packet(user_id, user_id, CON_TYPE_ROTATE); // 앞이 돌아갔다는 정보 받을애, 뒤에가 실제로 돌아간애, 일단 내가 나 돌아간거 알림
    send_fix_packet(user_id, user_id);
    for (auto cpy_vl : copy_viewlist) // 움직인 이후의 시야 범위에 대하여
    {
        if (is_player(cpy_vl))
        {
            send_condition_packet(cpy_vl, user_id, CON_TYPE_ROTATE); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
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
    // 복사본 뷰리스트에 다른 쓰레드가 접근하면 어쩌냐? 그 정도는 감수해야함
    c.m_cLock.unlock();

    send_condition_packet(user_id, user_id, CON_TYPE_MOVE); // 앞이 돌아갔다는 정보 받을애, 뒤에가 실제로 돌아간애, 일단 내가 나 돌아간거 알림
    send_fix_packet(user_id, user_id);
    for (auto cpy_vl : copy_viewlist) // 움직인 이후의 시야 범위에 대하여
    {
        if (is_player(cpy_vl))
        {
            send_condition_packet(cpy_vl, user_id, CON_TYPE_MOVE); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
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
    SESSION& n = g_clients[npc_id];
    for (int i = 0; i < g_clients[n.m_owner_id].m_boid.size(); ++i)
    {
        if (g_clients[n.m_owner_id].m_boid[i].id == n.m_id)
        {
            _vec3* n_pos = n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3* n_look = n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
            _vec3* p_pos = g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
            _vec3* p_look = g_clients[n.m_owner_id].m_transform.Get_StateInfo(CTransform::STATE_LOOK);
            if (*n_pos != g_clients[n.m_owner_id].m_boid[i].final_pos) // 만약 자신의 최종 위치가 아닐때
            {
                if (dist_between(npc_id, n.m_owner_id) > dist_between_finalPos(n.m_owner_id, i) + 0.1f
                    || dist_between(npc_id, n.m_owner_id) < dist_between_finalPos(n.m_owner_id, i) - 0.1f)// 자신의 포메이션 반지름 밖일때
                {
                    n.m_anim = A_WALK;
                    n.m_Mcondition = CON_STRAIGHT;
                    n.m_transform.BackWard(MOVE_TIME_ELAPSE);
                    _vec3 finalLookAt = g_clients[n.m_owner_id].m_boid[i].final_pos - *n_pos;
                    finalLookAt = Vector3_::Normalize(finalLookAt);
                    _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look);

                    _vec3 set_pos = {};

                    float PdotProduct = (npcLookAt.x * finalLookAt.x) + (npcLookAt.y * finalLookAt.y) + (npcLookAt.z * finalLookAt.z); // 내각
                    _vec3 PoutProduct;
                    PoutProduct.x = (finalLookAt.y * npcLookAt.z) - (finalLookAt.z * npcLookAt.y); // 외각
                    PoutProduct.y = (finalLookAt.z * npcLookAt.x) - (finalLookAt.x * npcLookAt.z);
                    PoutProduct.z = (finalLookAt.x * npcLookAt.y) - (finalLookAt.y * npcLookAt.x);

                    float radian = acosf(PdotProduct); // 내각 이용한 각도 추출
                    if (PoutProduct.y < 0)
                        radian *= -1.f;
                    float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
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
                else // 자신의 포메이션 반지름과 같거나 이내일때
                {
                    if (n.m_isOut) // 최초로 반지름 범위에 들어왔을때
                    {
                        n.m_isOut = false;
                        n.m_Mcondition = CON_IDLE;
                        n.m_Rcondition = CON_IDLE;
                        _vec3 standard = -1.f * Vector3_::Normalize(*p_look);
                        _vec3 toNpc = *n_pos - *p_pos;
                        toNpc = Vector3_::Normalize(toNpc);

                        _vec3 set_pos = {};

                        float PdotProduct = (toNpc.x * standard.x) + (toNpc.y * standard.y) + (toNpc.z * standard.z); // 내각
                        _vec3 PoutProduct;
                        PoutProduct.x = (standard.y * toNpc.z) - (standard.z * toNpc.y); // 외각
                        PoutProduct.y = (standard.z * toNpc.x) - (standard.x * toNpc.z);
                        PoutProduct.z = (standard.x * toNpc.y) - (standard.y * toNpc.x);

                        float radian = acosf(PdotProduct); // 내각 이용한 각도 추출
                        if (PoutProduct.y < 0)
                            radian *= -1.f;
                        float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
                        if (NPCangle > 0)  NPCangle += 3.f;
                        else if (NPCangle < 0) NPCangle -= 3.f;
                        n.m_total_angle = g_clients[n.m_owner_id].m_total_angle + NPCangle;
                        n_pos->x = g_clients[n.m_owner_id].m_boid[i].radius * sinf((n.m_total_angle) * (PIE / 180.f)) + p_pos->x;
                        n_pos->z = g_clients[n.m_owner_id].m_boid[i].radius * cosf((n.m_total_angle) * (PIE / 180.f)) + p_pos->z;
                        {
                            for (int i = 0; i < NPC_START; ++i) // npc 시야범위 내 있는 플레이어들에게 신호 보내는 곳
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
                    else if (!n.m_isOut && !n.m_isFormSet) // 최초로 반지름 들어온건 아닌데 방향셋팅 안되어있을때
                    {
                        n.m_Mcondition = CON_IDLE;
                        _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look); // npc가 바라보는 방향

                        _vec3 toNpc = *n_pos - *p_pos; // 플레이어에서 npc 바라보는 방향
                        toNpc = Vector3_::Normalize(toNpc);

                        float PdotProduct = (npcLookAt.x * toNpc.x) + (npcLookAt.y * toNpc.y) + (npcLookAt.z * toNpc.z); // 내각
                        float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

                        float PoutProduct = (toNpc.x * npcLookAt.z) - (toNpc.z * npcLookAt.x); // 앞에 x 벡터 기준 각도 차이
                        if (PoutProduct > 0) // 양수이면 npcLookAt는 toNpc로 부터 반시계
                            radian *= -1.f;

                        float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
                        if (-0.03f > PdotProduct || 0.03f < PdotProduct) // 나아가야할 수직 방향을 안바라볼때
                        {
                            if (g_clients[n.m_owner_id].m_boid[i].angle > n.m_total_angle) // 현재 npc 각도보다 가야할 포메이션 각도가 더 클때, 차이가 플러스
                            {
                                n.m_Rcondition = CON_LEFT;
                                n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE); // 왼쪽회전
                            }
                            else //차이가 마이너스
                            {
                                n.m_Rcondition = CON_RIGHT;
                                n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE); // 오른쪽
                            }
                        }
                        else // 나아가야할 수직 방향을 바라볼때
                        {
                            n.m_Rcondition = CON_IDLE;
                            n.m_isFormSet = true;
                            for (int i = 0; i < NPC_START; ++i) // npc 시야범위 내 있는 플레이어들에게 신호 보내는 곳
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
                    else // 방향 셋팅까지 다 완료 되었을때
                    {
                        if (g_clients[n.m_owner_id].m_boid[i].angle + 3.f < n.m_total_angle
                            || g_clients[n.m_owner_id].m_boid[i].angle - 3.f > n.m_total_angle) // 자기가 있어야할 각도 위치가 아닐경우
                        {
                            _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look); // npc가 바라보는 방향

                            _vec3 toNpc = *n_pos - *p_pos; // 플레이어에서 npc 바라보는 방향
                            toNpc = Vector3_::Normalize(toNpc);



                            float PdotProduct = (npcLookAt.x * toNpc.x) + (npcLookAt.y * toNpc.y) + (npcLookAt.z * toNpc.z); // 내각
                            float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

                            float PoutProduct = (toNpc.x * npcLookAt.z) - (toNpc.z * npcLookAt.x); // 앞에 x 벡터 기준 각도 차이
                            if (PoutProduct > 0) // 양수이면 npcLookAt는 toNpc로 부터 반시계
                                radian *= -1.f;

                            float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
                            if (-0.03f > PdotProduct || 0.03f < PdotProduct) // 나아가야할 수직 방향을 안바라볼때
                            {
                                n.m_anim = A_WALK;
                                n.m_Mcondition = CON_IDLE;
                                if (g_clients[n.m_owner_id].m_boid[i].angle < n.m_total_angle) // 현재 npc 각도보다 가야할 포메이션 각도가 더 클때, 차이가 플러스
                                {
                                    n.m_Rcondition = CON_LEFT;
                                    n.m_transform.Rotation_Y(-ROTATE_TIME_ELAPSE); // 왼쪽회전
                                }
                                else //차이가 마이너스
                                {
                                    n.m_Rcondition = CON_RIGHT;
                                    n.m_transform.Rotation_Y(ROTATE_TIME_ELAPSE); // 오른쪽
                                }
                            }
                            else
                            {
                                _vec3 lastPos = *n_pos; // 한발자국 나아가기 전에 마지막 위치 계산
                                n.m_Mcondition = CON_STRAIGHT;
                                n.m_Rcondition = CON_IDLE;
                                n.m_transform.BackWard(MOVE_TIME_ELAPSE); // 한 발자국 앞으로 나아감
                                 // 각도 비교후 바뀐 각도만큼 토탈 앵글 계산

                                _vec3 newLook = *n_pos - *p_pos;
                                newLook = Vector3_::Normalize(newLook);
                                _vec3 oldLook = lastPos - *p_pos;
                                oldLook = Vector3_::Normalize(oldLook);

                                PdotProduct = (newLook.x * oldLook.x) + (newLook.y * oldLook.y) + (newLook.z * oldLook.z); // 내각
                                _vec3 PoutProduct;
                                PoutProduct.x = (oldLook.y * newLook.z) - (oldLook.z * newLook.y); // 외각
                                PoutProduct.y = (oldLook.z * newLook.x) - (oldLook.x * newLook.z);
                                PoutProduct.z = (oldLook.x * newLook.y) - (oldLook.y * newLook.x);
                                float radian = acosf(PdotProduct); // 내각 이용한 각도 추출
                                if (PoutProduct.y < 0)
                                    radian *= -1.f;
                                float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지
                                n.m_total_angle += NPCangle;
                            }
                        }
                        else // 자기가 있어야할 최종 위치일때
                        {
                            n.m_Mcondition = CON_IDLE;
                            _vec3 standard = -1.f * Vector3_::Normalize(*p_look);
                            _vec3 npcLookAt = -1.f * Vector3_::Normalize(*n_look);

                            float PdotProduct = (npcLookAt.x * standard.x) + (npcLookAt.y * standard.y) + (npcLookAt.z * standard.z); // 내각
                            float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

                            float PoutProduct = (standard.x * npcLookAt.z) - (standard.z * npcLookAt.x); // 앞에 x 벡터 기준 각도 차이
                            if (PoutProduct > 0) // 양수이면 npcLookAt는 standard로 부터 반시계
                                radian *= -1.f;

                            float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지

                            if (NPCangle > 2.f || NPCangle < -2.f) // npc가 바라보는 방향이 플레이어랑 일치하지 않을때
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
            //    if (false == is_near(player, i)) // 근처에 없는애면 보내지도 마라
            //        continue;

            //    send_rotate_packet(player, i); // 내 시야범위 안에 있는 애들한테만 내가 돌아갔다는거 보냄
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

            Dir = Dir / hyp; // 여기가 노멀값
            Dir = Dir * 0.1f; // 노멀값 방향으로 얼만큼 갈지 계산
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
        //case FUNC_NPC_RANDMOVE:
        //{
        //    do_random_move(npc_id);
        //}
        //break;
        }
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
                g_clients[i].m_cLock.unlock(); // 여기 아마 잘못했을거임
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
        add_timer(npc_id, g_clients[npc_id].m_last_order, FRAME_TIME); // 생성 이후 반복 간격
}

void Server::dead_reckoning(int player_id, ENUM_FUNCTION func_id)
{
    bool isMove = false;
    if (ST_ACTIVE == g_clients[player_id].m_status) // NPC를 소유한 플레이어가 활성화 되어 있을때
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

    for (auto& c : g_clients) // aabb 충돌체크
    {
        if (c.second.m_id == player_id)
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (ST_DEAD == c.second.m_status)
            continue;
        if (false == is_near(c.second.m_id, player_id)) // 근처에 없는애는 그냥 깨우지도 마라
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
    // 복사본 뷰리스트에 다른 쓰레드가 접근하면 어쩌냐? 그 정도는 감수해야함
    g_clients[player_id].m_cLock.unlock();
    unordered_set<int> new_viewlist;

    for (auto& c : g_clients)
    {
        if (c.second.m_id == player_id)
            continue;
        if (false == is_near(c.second.m_id, player_id)) // 근처에 없는애는 그냥 깨우지도 마라
            continue;
        if (ST_ACTIVE != c.second.m_status)
            continue;
        new_viewlist.insert(c.second.m_id); // 내 시야 범위안에 들어오는 다른 객체들의 아이디를 주입
    }

    for (auto new_vl : new_viewlist) // 움직인 이후의 시야 범위에 대하여
    {
        if (0 == old_viewlist.count(new_vl)) // 이전 뷰리스트에 new_vl의 개수가 0개 일때 = 이전 시야에 없던 애일때
        {
            send_enter_packet(player_id, new_vl); // 다른 객체들의 정보를 나에게 전송

            if (false == is_player(new_vl)) // 새로 시야에 들어온 애가 플레이어가 아니면 걍 반복문 넘김 / 이 아니라 npc 발동해주고 넘김
            {
                continue;
            }

            g_clients[new_vl].m_cLock.lock();
            if (0 == g_clients[new_vl].m_view_list.count(player_id)) // 상대의 뷰리스트에 내가 없다면
            {
                g_clients[new_vl].m_cLock.unlock();
                send_enter_packet(new_vl, player_id); // 나의 입장 정보를 다른 객체들에게 전송
            }
            else
            {
                g_clients[new_vl].m_cLock.unlock();
                //send_move_packet(new_vl, player_id); // 나의 움직임 정보를 다른 객체들에게 전송
            }
        }
        else // 이동 한 후에 새 시야에 보이는 플레이어인데 이전에도 보였던 애다 = 기존 시야에 있던 애
        {
            if (false == is_player(new_vl)) // npc에게 내 새로운 움직임을 보내줄 필요가 없음
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

    for (auto old_vl : old_viewlist) // 움직이기 전 시야범위에 대하여
    {
        if (0 == new_viewlist.count(old_vl)) // 새 시야범위에 old_vl 갯수가 0일때 = 시야 범위에서 벗어난 객체일때
        {
            send_leave_packet(player_id, old_vl); // 나에게 상대 객체가 나갔다 알림

            if (false == is_player(old_vl)) // npc에게 내가 나갔다는거 안알려도 된다
                continue;

            g_clients[old_vl].m_cLock.lock();
            if (0 != g_clients[old_vl].m_view_list.count(player_id))
            {
                g_clients[old_vl].m_cLock.unlock();
                send_leave_packet(old_vl, player_id); // 상대 객체에게 내가 나갔다 알림
            }
            else // 실수하기 쉬움, else에 뭐 없더라고 unlock 해줄것, 안그러면 조건 불만족시 락 안풀림
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
            add_timer(player_id, g_clients[player_id].m_last_move, FRAME_TIME); // 생성 이후 반복 간격
            return;
        }
    }
    else
    {
        if (g_clients[player_id].m_last_rotate == g_clients[player_id].m_curr_rotate)
        {
            add_timer(player_id, g_clients[player_id].m_last_rotate, FRAME_TIME); // 생성 이후 반복 간격
        }
    }
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
    //packet.move_time = g_clients[mover].m_move_time; // 스트레스 테스트
    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
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

    //packet.move_time = g_clients[mover].m_move_time; // 스트레스 테스트
    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
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

            if (StartGame_PlayerCount < count)
            {
                //add_timer(-1, FUNC_CHECK_FLAG, 100);// 게임 플레이 시간 돌리는 함수
                //add_timer(-1, FUNC_CHECK_TIME, 1000);// 게임 플레이 시간 돌리는 함수
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
        g_clients[i].m_id = i; // 유저 등록
        g_clients[i].m_status = ST_FREE; // 여기는 멀티스레드 하기전에 싱글스레드일때 사용하는 함수, 락 불필요
    }
}

void Server::initialize_objects()
{
    for (int i = NPC_START; i < MAX_OBJECT; ++i)
    {
        g_clients[i].m_id = i; // 유저 등록
        g_clients[i].m_status = ST_FREE; // 여기는 멀티스레드 하기전에 싱글스레드일때 사용하는 함수, 락 불필요
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
    packet.con_move = g_clients[other_id].m_Mcondition;
    packet.con_rotate = g_clients[other_id].m_Rcondition;

    strcpy_s(packet.name, g_clients[other_id].m_name);

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

    send_packet(user_id, &packet); // 해당 유저에서 다른 플레이어 정보 전송
}

void Server::send_attacked_packet(int user_id, int other_id)
{
    sc_packet_attacked packet;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ATTACKED;
    packet.id = other_id;
    packet.hp = g_clients[other_id].m_hp;
    //cout << user_id << " saw " << other_id << " attacked\n";
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

void Server::send_chat_packet(int listen_id, int chatter_id, char mess[])
{
    sc_packet_chat packet;
    packet.id = chatter_id; // 채팅 보내는 사람들의
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_CHAT;
    strcpy_s(packet.message, mess);

    send_packet(listen_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
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
    //cout << idler << " do idle\n";
    send_packet(user_id, &packet); // 패킷 통채로 넣어주면 복사되서 날라가므로 메모리 늘어남, 성능 저하, 주소값 넣어줄것
}

void Server::do_attack(int npc_id)
{
    SESSION& n = g_clients[npc_id];
    _vec3 n_look = *n.m_transform.Get_StateInfo(CTransform::STATE_LOOK);
    n_look = -1.f * Vector3_::Normalize(n_look);
    _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);

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
            // 활성화 상태이고 내 팀이 아니고 공격범위 안에 있는 상대일때
            n.m_attack_target = i;
            //for (int i = 0; i < NPC_START; ++i) // npc 시야범위 내 있는 플레이어들에게 신호 보내는 곳
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

        if (n.m_attack_target >= 0) // 주변에 적이 있으면 탐색 멈추고 바로 전투
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

            if (NPCangle > 1.5f || NPCangle < -1.5f) // npc가 바라보는 방향이 플레이어랑 일치하지 않을때
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
            else // npc가 바라보는 방향이 플레이어랑 일치할때
            {
                n.m_Mcondition = CON_STRAIGHT;
                n.m_Rcondition = CON_IDLE;
                n.m_anim = A_WALK;
                n.m_transform.BackWard(MOVE_TIME_ELAPSE);
            }
        }
    }
    else // 공격할 대상이 지정이 되었을때
    {
        _vec3 t_pos = *g_clients[n.m_attack_target].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
        _vec3 n_pos = *n.m_transform.Get_StateInfo(CTransform::STATE_POSITION);
        _vec3 t_look = t_pos - n_pos;
        t_look = Vector3_::Normalize(t_look);

        float PdotProduct = (n_look.x * t_look.x) + (n_look.y * t_look.y) + (n_look.z * t_look.z); // 내각
        float radian = acosf(PdotProduct); // 내각 이용한 각도 추출

        float PoutProduct = (t_look.x * n_look.z) - (t_look.z * n_look.x); // 앞에 x 벡터 기준 각도 차이
        if (PoutProduct > 0) // 양수이면 n_look는 t_look로 부터 반시계
            radian *= -1.f;

        float NPCangle = radian * 180.f / PIE; // 현재 npc 위치가 플레이어 기준 몇도 차이나는지

        if (NPCangle > 1.5f || NPCangle < -1.5f) // npc가 공격할 대상을 안바라볼때
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
        else // npc가 공격할 대상을 바라볼때
        {
            if (is_attackable(n.m_id, n.m_attack_target)) // 전투 범위 안에 들어왔을때
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
        if (is_near(id, i)) // 죽은애 주변에 있는 플레이어에게
        {
            send_dead_packet(i, id); // 깎이고 남은 체력 있으면
            send_animation_packet(i, id, A_DEAD);
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

void Server::do_battle(int id)
{
    SESSION& att = g_clients[id];
    g_clients[att.m_attack_target].m_hp -= ATTACK_DAMAGE;
    if (g_clients[att.m_attack_target].m_hp <= 0) // 죽은 상태면
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
            // 활성화 되어있고 맞은애 시야범위 안에 있는 유저일때
            send_leave_packet(i, att.m_attack_target); // 남은 체력 브로드캐스팅
        }
        att.m_attack_target = -1;
    }
    else // 맞은 이후에 체력이 남아있는 상태면
    {
        for (int i = 0; i < NPC_START; ++i)
        {
            if (ST_ACTIVE != g_clients[i].m_status)
                continue;
            if (!is_near(i, att.m_attack_target))
                continue;
            // 활성화 되어있고 맞은애 시야범위 안에 있는 유저일때
            send_attacked_packet(i, att.m_attack_target); // 남은 체력 브로드캐스팅
        }
        if (!is_player(id))
            add_timer(id, FUNC_BATTLE, 1000); // 1초뒤에 또 공격
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
                g_clients[user_id].m_owner_id = user_id; // 유저 등록
                g_clients[user_id].m_view_list.clear(); // 이전 뷰리스트 가지고 있으면 안되니 초기화
                g_clients[user_id].m_isFighting = false;
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
        case FUNC_PLAYER_MOVE_FOR_NPC: // API_Send_message 호출용
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

bool Server::check_basic_collision(int a, int b)
{
    _vec3* a_pos = g_clients[a].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    _vec3* b_pos = g_clients[b].m_transform.Get_StateInfo(CTransform::STATE_POSITION);
    float a_rad = g_clients[a].m_col.radius;
    float b_rad = g_clients[b].m_col.radius;
    //_vec3 a_col = g_clients[a].m_col.col_range;
    //_vec3 b_col = g_clients[b].m_col.col_range;

    //_vec3 a_min = { a_pos->x - a_col.x / 2,a_pos->y ,a_pos->z - a_col.z / 2 }; // AABB 충돌
    //_vec3 a_max = { a_pos->x + a_col.x / 2,a_pos->y + a_col.y ,a_pos->z + a_col.z / 2 };
    //_vec3 b_min = { b_pos->x - b_col.x / 2,b_pos->y ,b_pos->z - b_col.z / 2 };
    //_vec3 b_max = { b_pos->x + b_col.x / 2,b_pos->y + b_col.y ,b_pos->z + b_col.z / 2 };

    //if ((a_min.x <= b_max.x && a_max.x >= b_min.x) &&
    //    (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
    //    (a_min.z <= b_max.z && a_max.z >= b_min.z))
    //{
    //    //Pos = PrevPos; 이전 위치로 되돌리기
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
            // 수정
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
//    //    //Pos = PrevPos; 이전 위치로 되돌리기
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