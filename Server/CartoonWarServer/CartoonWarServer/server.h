#pragma once

void do_move(int user_id, char direction);
void enter_game(int user_id);
void initalize_clients();
void disconnect(int user_id);

void recv_packet_construct(int user_id, int io_byte);
void process_packet(int user_id, char* buf);

void send_login_ok_packet(int user_id);
void send_enter_packet(int user_id, int other_id);
void send_move_packet(int user_id, int mover);
void send_leave_packet(int user_id, int other_id);
void send_packet(int user_id, void* packet);

void init_IOCP(SOCKET& sock, HANDLE& iocp);
void init_AcceptEx(SOCKET& sock, HANDLE& iocp, int l_key);
void close_server(SOCKET& sock);

void worker_thread();