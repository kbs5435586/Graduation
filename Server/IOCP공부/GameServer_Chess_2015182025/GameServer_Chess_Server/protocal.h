#pragma once

#include <WS2tcpip.h>
#pragma comment (lib,"Ws2_32.lib")

constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
constexpr int MAX_PLAYER = 10;

//constexpr char SERVER_ADDR[] = "127.0.0.1";

WSABUF wsabuf;
SOCKET c_socket;
char buff[BUF_SIZE];