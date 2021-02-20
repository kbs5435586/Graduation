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
    Player_Info g_client;
    unordered_map <int, Player_Info> g_npcs;
    SOCKET c_socket; // 서버와 연결할 소켓
    string client_IP; // 클라 IP
    short PORT = 3500;
public:
    void MainServer(CManagement* managment);
    BOOL InitServer(HWND hWnd);

    void ProcessPacket(char* ptr);
    void process_data(char* net_buf, size_t io_byte);

    void send_packet(void* packet);
    void send_move_packet(unsigned char dir);

    void err_quit(const char* msg);

private:
    virtual void Free();
};

