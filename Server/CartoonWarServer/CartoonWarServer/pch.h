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

constexpr float MOVE_SPEED = 0.3f;
constexpr float ROTATE_SPEED = 0.3f;
constexpr int FRAME_TIME = 17; // 1/4�ʿ� 1������, 60�������� 1/60�ʿ� 1�� ����, �뷫 16ms,17ms�ϸ� �ɵ�
constexpr float FORMATION_SPACE = 50.f;
