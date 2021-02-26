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
    int m_myid;
    Player_Info m_player;
    unordered_map <int, Player_Info> m_npcs;
    SOCKET m_cSocket; // 서버와 연결할 소켓
    string m_client_IP; // 클라 IP
    short PORT = 3500;
    CManagement* managment;
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

    bool Get_ShowPlayer();
    bool Get_ShowNPC();
    void err_quit(const char* msg);
    void disconnect();

private:
    virtual void Free();
};

