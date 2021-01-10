#pragma once
#include "pch.h"

#define MAX_BUFFER        1024
#define SERVER_PORT       3500

void mainServer();
void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);