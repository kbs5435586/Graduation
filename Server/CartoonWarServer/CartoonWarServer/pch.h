#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <chrono>
#include <queue>
#include <vector>
#include "directxmath.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
//#pragma comment(lib, "lua54.lib")

using namespace std;
using namespace chrono;

//extern "C"
//{
//#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
//}

#include "protocol.h"
#include "Struct.h"

constexpr float NPC_SPEED = 0.3;
constexpr int REPEAT_TIME = 17; // 1/4초에 1번전송, 60프레임은 1/60초에 1번 전송, 대략 16ms,17ms하면 될듯