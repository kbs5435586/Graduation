#include "pch.h"

void do_move(ClientInfo* g_clients, int user_id, char direction)
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
        if (y < WORLD_HEIGHT - 1)
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

    for (auto& c : g_clients)
    {
        if (true == c.m_isConnected)
        {
            send_move_packet(c.m_id, user_id); // 연결된 모든 클라이언트들에게 움직인 클라의 위치값 전송
        }
    }
}

void enter_game(int user_id)
{
    g_clients[user_id].m_isConnected = true;
    for (int i = 0; i < MAX_USER; i++)
    {
        if (true == g_clients[i].m_isConnected) // 이미 연결 중인 클라들한테만
        {
            if (user_id != i) // 나 자신한텐 send_enter_packet 보낼 필요가 없음, 내가 들어왔다는걸 다른 클라에 알리는 패킷임
            {
                send_enter_packet(user_id, i); // 새로 접속한 클라에게 이미 연결중인 클라 정보들을 보냄 
                send_enter_packet(i, user_id); // 이미 접속한 플레이어들에게 새로 접속한 클라정보 보냄
            }

        }
    }
}

void initalize_clients()
{
    for (int i = 0; i < MAX_USER; ++i)
        g_clients[i].m_isConnected = false;
}

void disconnect(int user_id)
{
    g_clients[user_id].m_isConnected = false;
    for (auto& c : g_clients) // 연결되어있는 클라이언트들에게 떠난 클라가 나갔다고 알림
    {
        send_leave_packet(c.m_id, user_id);
    }
}