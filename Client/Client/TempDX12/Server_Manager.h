#pragma once
#include "Base.h"
class CManagement;
class CServer_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CServer_Manager);
private:
    CServer_Manager();
    virtual ~CServer_Manager() = default;

private:
    CManagement* managment;

    Player_Info m_player;
    unordered_map <int, Player_Info> m_npcs;

    SOCKET m_cSocket; // 서버와 연결할 소켓
    string m_client_IP; // 클라 IP
    short PORT = 3500;

    int m_myid;
    char last_order;
    high_resolution_clock::time_point cool_time;

public:
    void MainServer(CManagement* managment);
    BOOL InitServer(HWND hWnd);

    void ProcessPacket(char* ptr);
    void process_data(char* net_buf, size_t io_byte);
    void SocketEventMessage(HWND hWnd, LPARAM lParam);

    void send_packet(void* packet);
    void send_move_packet(unsigned char dir);
    void send_login_ok_packet();
    void send_add_npc_packet();
    void send_npc_act_packet(unsigned char act);

    bool Get_ShowPlayer();
    bool Get_ShowNPC();
    char Get_Lastorder();
    void Set_LastOrder(char order);
    high_resolution_clock::time_point Get_Cooltime();
    void Set_CoolTime(high_resolution_clock::time_point ct);

    void err_quit(const char* msg);
    void disconnect();

private:
    virtual void Free();
};

