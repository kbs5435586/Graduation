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
    wcout << L"���� " << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
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

	send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
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

    g_clients[user_id].m_cLock.lock();
    unordered_set<int> old_viewlist = g_clients[user_id].m_view_list;
    // ���纻 �丮��Ʈ�� �ٸ� �����尡 �����ϸ� ��¼��? �� ������ �����ؾ���
    g_clients[user_id].m_cLock.unlock();
    unordered_set<int> new_viewlist;

    for (auto& c : g_clients)
    {
        if (false == is_near(c.second.m_id, user_id)) // ��ó�� ���¾ִ� �׳� �������� ����
            continue;
        if (ST_SLEEP == c.second.m_status) // ��ó�� �ִ� npc�̸� ������
            activate_npc(c.second.m_id);
        if (ST_ACTIVE != c.second.m_status)
            continue;
        if (c.second.m_id == user_id)
            continue;
        if (false == is_player(c.second.m_id)) // �÷��̾ �ƴ� npc�̸�
        {
            OverEx* overEx = new OverEx;
            overEx->function = FUNC_PLAYER_MOVE;
            overEx->player_id = user_id;
            PostQueuedCompletionStatus(g_iocp, 1, c.second.m_id, &overEx->over);
        }
        
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

void Server::do_AI()
{
    while (true)
    {
        auto ai_start_time = high_resolution_clock::now();
        for (int i = NPC_ID_START; i < NPC_ID_START + MAX_NPC; ++i)
        {
            if ((high_resolution_clock::now() - g_clients[i].m_last_move_time) > 1s) // ���ݿ��� ���������� ������ �ð��� 1�ʰ� �Ǹ� �ٽ� �̵�
            {
                random_move_npc(i);
                g_clients[i].m_last_move_time = high_resolution_clock::now();
            }
        }
        auto ai_finish_time = high_resolution_clock::now() - ai_start_time;
        cout << "all AI move time : " << duration_cast<milliseconds>(ai_finish_time).count() << "ms\n"; // ���� �˻��غ���
    }
}

void Server::random_move_npc(int npc_id)
{
    int x = g_clients[npc_id].m_x;
    int y = g_clients[npc_id].m_y;
    switch (rand() % 4)
    {
    case MV_RIGHT: 
        if (x < (WORLD_WIDTH - 1))
            x++;
        break;
    case MV_LEFT:
        if (x > 0)
            x--;
        break;
    case MV_DOWN:
        if (y < (WORLD_HEIGHT - 1))
            y++;
        break;
    case MV_UP:
        if (y > 0)
            y--;
        break;
    }

    g_clients[npc_id].m_x = x;
    g_clients[npc_id].m_y = y;

    for (int i = 0; i < NPC_ID_START; ++i)
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

    // �ٵ� ���� �÷��̾ ������, �÷��̾� �� ����Ʈ �����Ҷ� npc���� ����ؼ� �� ����Ʈ ��������� �ǹǷ� ó������ ������ �丮��Ʈ �� ���������
}

void Server::activate_npc(int npc_id)
{
    ENUM_STATUS old_status = ST_SLEEP;
    if (true == atomic_compare_exchange_strong(&g_clients[npc_id].m_status, &old_status, ST_ACTIVE)) // m_status�� �������� ��Ƽ��� �ٲ� ��쿡��
        // ���ÿ� �� Ŭ�� �����ϸ� ACTIVE �� 2�� �ٲ�� Ÿ�̸Ӱ� 2�� �ߵ��ϴ°� �����ϱ� ���� �뵵
        add_timer(npc_id, FUNC_RANDMOVE, 1000);
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
        this_thread::sleep_for(1ms); // busy waiting ���� �� �ٸ� �����忡�� cpu �纸, 1�ʸ��� �˻��ض�, ��� �ϰ����� ����
        while (true) // ���� �ð��� �Ȱ� ������ ��� �������ִ� ��
        {
            timer_lock.lock();
            if (true == timer_queue.empty()) // Ÿ�̸� ť�� �ƹ��͵� ������
            {
                timer_lock.unlock();
                break; // ���� �ð��� �ȵ����� ���� ������ 1�� �����
            }
            if (timer_queue.top().wakeup_time > high_resolution_clock::now()) // wakeup_time�� ���ݺ��� ũ�� ���� ť���� �������� �ƴϴ�
            {
                // �ٵ� �̷��� ���������� <busy waiting : ������ ������ ������ �ݺ����� �����ϸ� ��ٸ��� ���> �߻���
                timer_lock.unlock();
                break; // ���� �ð��� �ȵ����� ���� ������ 1�� �����
            }

            event_type event = timer_queue.top(); // �̷��� �ϸ� �޸� ���� �Ͼ, ������� Ŀ��, �׷��� ť�� �ڷ����� �����ͷ� ������
            // �׷��� �ؼ� �߻��ϴ� new ��ü�� ������� �ƴϳ�, ����, �׷��� free list �Ἥ ���� ������� -> �˾Ƽ� �Ұ�
            timer_queue.pop();
            timer_lock.unlock();

            switch (event.event_id)
            {
            case FUNC_RANDMOVE:
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

void Server::send_move_packet(int user_id, int mover)
{
    sc_packet_move packet;
    packet.id = mover;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;
    packet.x = g_clients[mover].m_x;
    packet.y = g_clients[mover].m_y; // �̵��� �÷��̾��� ���� ���
    packet.move_time = g_clients[mover].m_move_time;

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

    for (auto& c : g_clients)
    {
        int i = c.second.m_id;

        if (user_id == i) // ����� ȸ�ǿ�
            continue;

        if (true == is_near(user_id, i))
        {
            //g_clients[i].m_cLock.lock();
            if (ST_SLEEP == g_clients[i].m_status)
            {
                activate_npc(i);
            }
            if (ST_ACTIVE == g_clients[i].m_status) // �̹� ���� ���� Ŭ������׸�, m_status�� ���� �ɾ�� ������
            {
                send_enter_packet(user_id, i); // ���� ������ Ŭ�󿡰� �̹� �������� Ŭ�� �������� ���� 
                if (true == is_player(i))
                    send_enter_packet(i, user_id); // �̹� ������ �÷��̾�鿡�� ���� ������ Ŭ������ ����
            }
            //g_clients[i].m_cLock.unlock();
        }
    }
}

void Server::initalize_clients()
{
    for (int i = 0; i < MAX_USER; ++i)
    {
        g_clients[i].m_id = i; // ���� ���
        g_clients[i].m_status = ST_FREE; // ����� ��Ƽ������ �ϱ����� �̱۽������϶� ����ϴ� �Լ�, �� ���ʿ�
    }
}

void Server::initalize_NPC()
{
    for (int i = NPC_ID_START; i < NPC_ID_START + MAX_NPC; ++i)
    {
        g_clients[i].m_socket = 0;
        g_clients[i].m_id = i;
        sprintf_s(g_clients[i].m_name, "NPC %d", i);
        g_clients[i].m_status = ST_SLEEP;
        g_clients[i].m_x = rand() % WORLD_WIDTH;
        g_clients[i].m_y = rand() % WORLD_HEIGHT;
        //g_clients[i].m_last_move_time = high_resolution_clock::now();
        //add_timer(i, FUNC_RANDMOVE, 1000);
        lua_State* L = g_clients[i].m_lua = luaL_newstate();
        luaL_openlibs(L);
        luaL_loadfile(L, "NPC.LUA"); // NPC.LUA ���� �ҷ�����
        lua_pcall(L, 0, 0, 0);
        lua_getglobal(L, "set_uid"); // set_uid �Լ��� ���ؿ� �ε�
        lua_pushnumber(L, i);
        lua_pcall(L, 1, 0, 0);
        lua_pop(L, 1); // �Լ� ȣ�� ������ �� �ؼ� lua_getglobal�� ȣ���Ѱ� ������

        lua_register(L, "API_send_message", API_SendMessage);
        lua_register(L, "API_get_x", API_get_x);
        lua_register(L, "API_get_y", API_get_y);
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
    packet.o_type = O_HUMAN; // �ٸ� �÷��̾���� ���� ����

    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_view_list.insert(other_id);
    g_clients[user_id].m_cLock.unlock();

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

void Server::disconnect(int user_id)
{
    send_leave_packet(user_id, user_id); // �� �ڽ�
    g_clients[user_id].m_cLock.lock();
    g_clients[user_id].m_status = ST_ALLOC; // ���⼭ free �ع����� �Ʒ����� �����߿� �ٸ� Ŭ�� �Ҵ�ɼ��� ����
    closesocket(g_clients[user_id].m_socket);

    for (int i = 0; i < NPC_ID_START; ++i)
    {
        if (user_id == g_clients[i].m_id)
            continue;

        //c.second.m_cLock.lock();
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
    if (abs(g_clients[a].m_x - g_clients[b].m_x) > VIEW_RADIUS) // abs = ���밪
        return false;
    if (abs(g_clients[a].m_y - g_clients[b].m_y) > VIEW_RADIUS)
        return false;
    // �̰� 2D �����̴ϱ� ����� �������� �� �簢���̹Ƿ� �簢�� �������� �þ߹��� ���
    // 3D ������ ��Ʈ(x-x�� ���� + y-y�� ����)> VIEW_RADIUS �̸� false�� ó���ؾ���
    return true;
}

bool Server::is_player(int id)
{
    return id < NPC_ID_START;
}

int Server::API_SendMessage(lua_State* L)
{
    int my_id = (int)lua_tointeger(L, -3);
    int user_id = (int)lua_tointeger(L, -2);
    char* mess = (char*)lua_tostring(L, -1);

    send_chat_packet(user_id, my_id, mess);
    lua_pop(L, 3);
    return 0;
}

int Server::API_get_x(lua_State* L)
{
    int obj_id = (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    int x = g_clients[obj_id].m_x;
    lua_pushnumber(L, x);
    return 1;
}

int Server::API_get_y(lua_State* L)
{
    int obj_id = (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    int y = g_clients[obj_id].m_y;
    lua_pushnumber(L, y);
    return 1;
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
            for (int i = 0; i < MAX_USER; ++i)
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
                g_clients[user_id].m_x = rand() % WORLD_WIDTH;
                g_clients[user_id].m_y = rand() % WORLD_HEIGHT;
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
        case FUNC_RANDMOVE:
        {
            random_move_npc(id);
            bool keep_alive = false;
            for (int i = 0; i < NPC_ID_START; ++i) // ��� �÷��̾ ���ؼ�
            {
                if (true == is_near(id, i)) // �÷��̾� �þ߹��� �ȿ� �ְ�
                {
                    if (ST_ACTIVE == g_clients[i].m_status) // �������ִ� �÷��̾��϶�
                    {
                        keep_alive = true; // npc�� Ȱ��ȭ �Ǿ��ִ�
                        break;
                    }
                }
            }

            if (true == keep_alive) // ó�� ���� �÷��̾� �������� Ȱ��ȭ �ߺ� ����
                add_timer(id, FUNC_RANDMOVE, 1000);
            else
                g_clients[id].m_status = ST_SLEEP; // �ֺ��� �÷��̾� ������ �ٽ� ���� ����

            delete overEx;
        }
            break;
        case FUNC_PLAYER_MOVE:
        {
            g_clients[id].m_lLock.lock();
            lua_State* L = g_clients[id].m_lua;
            lua_getglobal(L, "event_player_move");
            lua_pushnumber(L, overEx->player_id);
            lua_pcall(L, 1, 0, 0);
            lua_pop(L, 1);
            g_clients[id].m_lLock.unlock();
            delete overEx;
        }


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

    cout << "NPC Init Start\n";
    initalize_NPC();
    cout << "NPC Init Finish\n";


    listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // �׳� ���ε� ���� C++11�� �ִ� �����Լ��� ����
    listen(listenSocket, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // Ŀ�� ��ü ����, IOCP ��ü ����
    initalize_clients(); // Ŭ���̾�Ʈ ������ �ʱ�ȭ

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

    thread timer_thread([this]() {this->do_timer(); });

    for (auto& t : worker_threads)
    {
        t.join();
    }
    closesocket(listenSocket);
    WSACleanup();
}