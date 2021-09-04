#pragma once
#include "Base.h"
class CManagement;
class CTransform;
class CServer_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CServer_Manager);
private:
    CServer_Manager();
    virtual ~CServer_Manager() = default;

private:
    CManagement* managment;
    unordered_map <int, Session> m_objects;
    Flag flags[5];
    short my_id = 0;
    short my_npc;
    short my_hp;
    short my_troop;
    short my_last_troop;
    bool isSendOnePacket;
    bool isConnected = false;
    bool isRed;
    bool isBlue;
    string m_ip;

    WSAEVENT m_EventArray;

    SOCKET m_cSocket; // 서버와 연결할 소켓
    string m_client_IP; // 클라 IP
    short PORT = 3500;
    int game_time = 0;
    float time_delta = 0.f;
    WPARAM m_wparam;
    high_resolution_clock::time_point add_npc_ct;
    high_resolution_clock::time_point attack_ct;
    high_resolution_clock::time_point change_formation_ct;
    high_resolution_clock::time_point move_ct;
    high_resolution_clock::time_point rotate_ct;

public:
    BOOL InitServer(HWND hWnd);

    void ProcessPacket(char* ptr);
    void process_data(char* net_buf, size_t io_byte);
    HRESULT EventManager();

    void send_packet(void* packet);
    void send_move_packet(unsigned char dir);
    void send_login_ok_packet();
    void send_add_npc_packet();
    void send_npc_act_packet(unsigned char act);
    void send_change_formation_packet();
    void send_animation_packet(unsigned char anim);
    void send_attack_packet();
    void send_position_packet(_vec3* pos);
    void send_class_change_packet(int idx, char type);
    void send_change_troop();
    void send_fire_packet(float mx, float mz);
    void send_teleport_packet(float mx, float mz);
    void send_arrow_packet();
    void send_invisible_packet(bool isinvi);
    void send_deffend_packet();

    void update_key_input();
    void update_anim(int id, unsigned char anim);
    void Set_Server_Mat(int id, _matrix* mat);

    short npc_idx_to_id(unsigned short id);
    short npc_id_to_idx(unsigned short id);
    short object_idx_to_id(unsigned short id);
    short object_id_to_idx(unsigned short id);

    bool Get_Particle(int id, char type);
    bool Get_Show(int index, char type);
    bool Get_ShowPlayer();
    bool Get_Blue(int id);
    bool Get_Red(int id);
    bool Get_Connected();
    short Get_PlayerID();
    short Get_HP(int id, char type);
    int Get_PlayerClass(int id);
    int Get_NpcClass(int id);
    _matrix Get_ServerMat(int id, char type);

    short Get_ShowOtherPlayer(int id);
    short Get_Anim(int id, char type);
    float Get_GameTime();
    short Get_NpcSize();
    short Get_TroopClass();
    bool Get_isInvisible(int id);
    float Get_TimeDelta();
    bool Get_isFirst(int id, char type);

    void Set_Move_CoolTime(high_resolution_clock::time_point ct);
    void Set_Rotate_CoolTime(high_resolution_clock::time_point ct);
    void Set_TroopClass(short mclass);
    void Set_Anim(short anim, int id, char type);
    void Set_isFirst(bool first, int id, char type);

    bool is_player(int id);
    bool is_npc(int id);
    bool is_object(int id);

    high_resolution_clock::time_point Get_Move_Cooltime();
    high_resolution_clock::time_point Get_Rotate_Cooltime();
    high_resolution_clock::time_point Get_AddNPC_Cooltime();
    high_resolution_clock::time_point Get_Attack_Cooltime();
    high_resolution_clock::time_point Get_ChangeFormation_Cooltime();
    WPARAM Get_wParam();

    void Set_Class(int mclass, int id, char type);
    void Set_AddNPC_CoolTime(high_resolution_clock::time_point ct);
    void Set_Attack_CoolTime(high_resolution_clock::time_point ct);
    void Set_ChangeFormation_CoolTime(high_resolution_clock::time_point ct);
    void Set_wParam(WPARAM p);
    void Set_Anim(unsigned short anim);
    void Set_Particle(int id, bool stat, char type);

    void init_client();

    void err_quit(const char* msg);
    void disconnect();

private:
    virtual void Free();
};