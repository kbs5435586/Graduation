#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "server.h"

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
constexpr int MAX_BUFFER = 4096;

constexpr char OP_MODE_RECV = 0;
constexpr char OP_MODE_SEND = 1;
constexpr char OP_MODE_ACCEPT = 2;

constexpr int  KEY_SERVER = 1000000;

struct OVER_EX {
    WSAOVERLAPPED wsa_over;
    char   op_mode;
    WSABUF   wsa_buf;
    unsigned char iocp_buf[MAX_BUFFER];
};

struct client_info {
    int id;
    char name[MAX_ID_LEN];
    short x, y;

    bool in_use;
    SOCKET   m_sock;
    OVER_EX   m_recv_over;
    unsigned char* m_packet_start;
    unsigned char* m_recv_start;
};

client_info g_clients[MAX_USER];
HANDLE      h_iocp;

SOCKET g_lSocket;
OVER_EX g_accept_over;

void send_packet(int id, void* p)
{
    unsigned char* packet = reinterpret_cast<unsigned char*>(p);
    OVER_EX* send_over = new OVER_EX;
    memcpy(send_over->iocp_buf, packet, packet[0]);
    send_over->op_mode = OP_MODE_SEND;
    send_over->wsa_buf.buf = reinterpret_cast<CHAR*>(send_over->iocp_buf);
    send_over->wsa_buf.len = packet[0];
    ZeroMemory(&send_over->wsa_over, sizeof(send_over->wsa_over));
    WSASend(g_clients[id].m_sock, &send_over->wsa_buf, 1, NULL, 0, &send_over->wsa_over, NULL);
}

void send_login_ok(int id)
{
    sc_packet_login_ok p;
    p.exp = 0;
    p.hp = 100;
    p.id = id;
    p.level = 1;
    p.size = sizeof(p);
    p.type = SC_PACKET_LOGIN_OK;
    p.x = g_clients[id].x;
    p.y = g_clients[id].y;
    send_packet(id, &p);
}

void send_move_packet(int id)
{
    sc_packet_move p;
    p.id = id;
    p.size = sizeof(p);
    p.type = SC_PACKET_MOVE;
    p.x = g_clients[id].x;
    p.y = g_clients[id].y;
    send_packet(id, &p);
}

void process_move(int id, char dir)
{
    short y = g_clients[id].y;
    short x = g_clients[id].x;
    switch (dir) {
    case MV_UP:
        if (y > 0)
            y--;
        break;
    case MV_DOWN:
        if (y < WORLD_HEIGHT - 1)
            y++;
        break;
    case MV_LEFT:
        if (x > 0)
            x--;
        break;
    case MV_RIGHT:
        if (x < WORLD_WIDTH - 1)
            x++;
        break;
    default:
        cout << "Unknown Direction in CS_MOVE packet.\n";
        while (true);
    }
    g_clients[id].x = x;
    g_clients[id].y = y;

    send_move_packet(id);
}

void process_packet(int id)
{
    char p_type = g_clients[id].m_packet_start[1]; // 패킷이 여기있음
    // p_type 은 패킷 타입
    switch (p_type)
    {
    case CS_LOGIN:
    {
        cs_packet_login* p = reinterpret_cast<cs_packet_login*>(g_clients[id].m_packet_start);
        strcpy_s(g_clients[id].name, p->name);
        send_login_ok(id);
    }
    break;
    case CS_MOVE:
    {
        cs_packet_move* p1 = reinterpret_cast<cs_packet_move*>(g_clients[id].m_packet_start);
        process_move(id, p1->direction);
    }
    break;
    default:
        cout << "Unknown Packet Type [" << p_type << "] from Client [" << id << "]\n";
        while (true);
    }
}

constexpr int MIN_BUFF_SIZE = 1024;

void process_recv(int id, DWORD iosize)
{
    unsigned char p_size = g_clients[id].m_packet_start[0];
    unsigned char* next_recv_ptr = g_clients[id].m_recv_start + iosize;
    while (p_size <= next_recv_ptr - g_clients[id].m_packet_start) {
        process_packet(id);
        g_clients[id].m_packet_start += p_size;
        if (g_clients[id].m_packet_start < next_recv_ptr)
            p_size = g_clients[id].m_packet_start[0];
        else break;
    }

    int left_data = next_recv_ptr - g_clients[id].m_packet_start;

    if ((MAX_BUFFER - (next_recv_ptr - g_clients[id].m_recv_over.iocp_buf))
        < MIN_BUFF_SIZE) {
        memcpy(g_clients[id].m_recv_over.iocp_buf,
            g_clients[id].m_packet_start, left_data);
        g_clients[id].m_packet_start = g_clients[id].m_recv_over.iocp_buf;
        g_clients[id].m_recv_start = g_clients[id].m_packet_start + left_data;
    }
    DWORD recv_flag = 0;
    WSARecv(g_clients[id].m_sock, &g_clients[id].m_recv_over.wsa_buf,
        1, NULL, &recv_flag, &g_clients[id].m_recv_over.wsa_over, NULL);
}

void add_new_client(SOCKET ns)
{
    int i;
    for (i = 0; i < MAX_USER; ++i)
        if (false == g_clients[i].in_use) break;

    if (MAX_USER == i)
    {
        cout << "Max user limit exceeded.\n";
        closesocket(ns);
    }
    else
    {
        g_clients[i].id = i;
        g_clients[i].in_use = true;
        g_clients[i].m_packet_start = g_clients[i].m_recv_over.iocp_buf;
        g_clients[i].m_recv_over.op_mode = OP_MODE_RECV;
        g_clients[i].m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(g_clients[i].m_recv_over.iocp_buf);
        g_clients[i].m_recv_over.wsa_buf.len = sizeof(g_clients[i].m_recv_over.iocp_buf);
        ZeroMemory(&g_clients[i].m_recv_over.wsa_over, sizeof(g_clients[i].m_recv_over.wsa_over));
        g_clients[i].m_recv_start = g_clients[i].m_recv_over.iocp_buf;
        g_clients[i].m_sock = ns;
        g_clients[i].name[0] = 0;
        g_clients[i].x = rand() % WORLD_WIDTH;
        g_clients[i].y = rand() % WORLD_HEIGHT;
        DWORD flags = 0;
        WSARecv(g_clients[i].m_sock, &g_clients[i].m_recv_over.wsa_buf, 1, NULL, &flags, &g_clients[i].m_recv_over.wsa_over, NULL);
    }
    SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    g_accept_over.op_mode = OP_MODE_ACCEPT;
    g_accept_over.wsa_buf.len = cSocket;
    ZeroMemory(&g_accept_over.wsa_over, 0, sizeof(g_accept_over.wsa_over));
    AcceptEx(g_lSocket, cSocket, g_accept_over.iocp_buf, 0, 16, 16, NULL, &g_accept_over.wsa_over);
}

int main()
{
    for (auto& cl : g_clients)
        cl.in_use = false;

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    g_lSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    CreateIoCompletionPort((HANDLE)g_lSocket, h_iocp, KEY_SERVER, 0);

    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    ::bind(g_lSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(g_lSocket, 5);

    SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    g_accept_over.op_mode = OP_MODE_ACCEPT;
    g_accept_over.wsa_buf.len = cSocket;
    ZeroMemory(&g_accept_over.wsa_over, 0, sizeof(g_accept_over.wsa_over));
    AcceptEx(g_lSocket, cSocket, g_accept_over.iocp_buf, 0, 16, 16, NULL, &g_accept_over.wsa_over);

    while (true) {
        DWORD io_size;
        ULONG_PTR key;
        WSAOVERLAPPED* lpover;
        int ret = GetQueuedCompletionStatus(h_iocp, &io_size, &key, &lpover, INFINITE);

        OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(lpover);
        switch (over_ex->op_mode) {
        case OP_MODE_ACCEPT:
            add_new_client(static_cast<SOCKET>(over_ex->wsa_buf.len));
            break;
        case OP_MODE_RECV:
            process_recv(key, io_size);
            break;
        case OP_MODE_SEND:
            delete over_ex; // send할때 사용한 오버를 반환, 릴리스 해야함
            break;
        }
    }
    closesocket(g_lSocket);
    WSACleanup();
}