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
    unordered_map <int, Player_Info> m_objects;
    Flag flags[5];
    short my_id;
    short my_hp;
    bool isSendOnePacket;
    bool isLogin;
    bool isRed;
    bool isBlue;
    string m_ip;

    SOCKET m_cSocket; // 서버와 연결할 소켓
    string m_client_IP; // 클라 IP
    short PORT = 3500;
    float game_time;
    WPARAM m_wparam;
    high_resolution_clock::time_point add_npc_ct;
    high_resolution_clock::time_point attack_ct;
    high_resolution_clock::time_point change_formation_ct;

public:
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
    void send_idle_packet();
    void send_attack_packet();
    void send_position_packet(_vec3* pos);
    void update_key_input();

    short player_index(unsigned short id);
    short npc_idx_to_id(unsigned short id);
    short npc_id_to_idx(unsigned short id);
    short object_idx_to_id(unsigned short id);
    short object_id_to_idx(unsigned short id);

    bool Get_ShowPlayer();
    bool Get_ShowNPC(int npc_index);
    bool Get_Blue(int id);
    bool Get_Red(int id);
    bool Get_Login();
    short Get_PlayerID();
    short Get_HP(int id);
    short Get_ShowOtherPlayer(int id);
    short Get_Anim(int id);
    float Get_GameTime();

    high_resolution_clock::time_point Get_AddNPC_Cooltime();
    high_resolution_clock::time_point Get_Attack_Cooltime();
    high_resolution_clock::time_point Get_ChangeFormation_Cooltime();
    WPARAM Get_wParam();

    void Set_AddNPC_CoolTime(high_resolution_clock::time_point ct);
    void Set_Attack_CoolTime(high_resolution_clock::time_point ct);
    void Set_ChangeFormation_CoolTime(high_resolution_clock::time_point ct);
    void Set_wParam(WPARAM p);

    void init_client();

    void err_quit(const char* msg);
    void disconnect();

private:
    virtual void Free();
};

