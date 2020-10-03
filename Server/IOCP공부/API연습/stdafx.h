// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN     // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#pragma comment (lib, "ws2_32.lib")
#include <WS2tcpip.h>
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>

using namespace std;

constexpr int MAX_LOADSTRING = 100;
constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;

constexpr int KEY_UP = 1;
constexpr int KEY_DOWN = 2;
constexpr int KEY_LEFT = 3;
constexpr int KEY_RIGHT = 4;

struct KeyState
{
	char key;
};

