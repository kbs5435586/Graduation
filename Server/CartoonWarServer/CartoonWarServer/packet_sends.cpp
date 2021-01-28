#include "pch.h"

void send_login_ok_packet(int user_id)
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

void send_enter_packet(int user_id, int other_id)
{
    sc_packet_enter packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_ENTER;
    packet.x = g_clients[other_id].m_x;
    packet.y = g_clients[other_id].m_y;
    strcpy_s(packet.name, g_clients[other_id].m_name);
    packet.o_type = O_PLAYER; // �ٸ� �÷��̾���� ���� ����

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void send_move_packet(int user_id, int mover)
{
    sc_packet_move packet;
    packet.id = mover;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_MOVE;
    packet.x = g_clients[mover].m_x;
    packet.y = g_clients[mover].m_y; // �̵��� �÷��̾��� ���� ���

    send_packet(user_id, &packet); // ��Ŷ ��ä�� �־��ָ� ����Ǽ� ���󰡹Ƿ� �޸� �þ, ���� ����, �ּҰ� �־��ٰ�
}

void send_leave_packet(int user_id, int other_id)
{
    sc_packet_leave packet;
    packet.id = other_id;
    packet.size = sizeof(packet);
    packet.type = SC_PACKET_LEAVE;

    send_packet(user_id, &packet); // �ش� �������� �ٸ� �÷��̾� ���� ����
}

void send_packet(int user_id, void* packet)
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