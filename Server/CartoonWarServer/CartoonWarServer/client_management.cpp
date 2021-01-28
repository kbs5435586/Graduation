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
            send_move_packet(c.m_id, user_id); // ����� ��� Ŭ���̾�Ʈ�鿡�� ������ Ŭ���� ��ġ�� ����
        }
    }
}

void enter_game(int user_id)
{
    g_clients[user_id].m_isConnected = true;
    for (int i = 0; i < MAX_USER; i++)
    {
        if (true == g_clients[i].m_isConnected) // �̹� ���� ���� Ŭ������׸�
        {
            if (user_id != i) // �� �ڽ����� send_enter_packet ���� �ʿ䰡 ����, ���� ���Դٴ°� �ٸ� Ŭ�� �˸��� ��Ŷ��
            {
                send_enter_packet(user_id, i); // ���� ������ Ŭ�󿡰� �̹� �������� Ŭ�� �������� ���� 
                send_enter_packet(i, user_id); // �̹� ������ �÷��̾�鿡�� ���� ������ Ŭ������ ����
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
    for (auto& c : g_clients) // ����Ǿ��ִ� Ŭ���̾�Ʈ�鿡�� ���� Ŭ�� �����ٰ� �˸�
    {
        send_leave_packet(c.m_id, user_id);
    }
}