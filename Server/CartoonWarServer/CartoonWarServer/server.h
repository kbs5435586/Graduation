#pragma once
#include "pch.h"

ClientInfo g_clients[MAX_USER];
int current_User_ID = 0;
HANDLE g_iocp;
constexpr int LISTEN_KEY = 999;

void mainServer();