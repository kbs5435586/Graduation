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
    high_resolution_clock::time_point add_npc_ct;
    high_resolution_clock::time_point change_formation_ct;
    high_resolution_clock::time_point select_ct;

public:
    void MainServer(CManagement* managment);
    BOOL InitServer(HWND hWnd);

    void ProcessPacket(char* ptr);
    void process_data(char* net_buf, size_t io_byte);
    void SocketEventMessage(HWND hWnd, LPARAM lParam);

    void send_packet(void* packet);
    void send_move_packet(unsigned char dir);
    void send_rotate_packet(unsigned char dir);
    void send_login_ok_packet();
    void send_add_npc_packet();
    void send_npc_act_packet(unsigned char act);
    void send_change_formation_packet();

    short player_index(unsigned short id);
    short npc_idx_to_id(unsigned short id);
    short npc_id_to_idx(unsigned short id);



    bool Get_ShowPlayer();
    bool Get_SelectPlayer();
    bool Get_ShowNPC(int npc_index);
    short Get_PlayerID();
    high_resolution_clock::time_point Get_AddNPC_Cooltime();
    high_resolution_clock::time_point Get_ChangeFormation_Cooltime();
    high_resolution_clock::time_point Get_Select_Cooltime();

    void Set_SelectPlayer(bool change);
    void Set_AddNPC_CoolTime(high_resolution_clock::time_point ct);
    void Set_ChangeFormation_CoolTime(high_resolution_clock::time_point ct);
    void Set_Select_CoolTime(high_resolution_clock::time_point ct);

    void err_quit(const char* msg);
    void disconnect();

private:
    virtual void Free();
};

