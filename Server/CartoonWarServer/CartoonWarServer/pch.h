#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <queue>

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <mutex>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;
using namespace chrono;

//extern "C"
//{
//#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
//}

#include "d3dmath.h"
#include "Transform.h"
#include "protocol.h"
#include "Struct.h"

constexpr float NPC_SPEED = 0.3;
constexpr int REPEAT_TIME = 100; // 1/4초에 1번전송, 60프레임은 1/60초에 1번 전송, 대략 16ms,17ms하면 될듯

