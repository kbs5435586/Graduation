// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

// directx
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

// visual leak detector
#include <vld.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "Constant.h"
#include "Define.h"
#include "Typedef.h"
#include "Extern.h"
#include "Function.h"
#include "Struct.h"

// Managers
#include "TimeMgr.h"
#include "AbstractFactory.h"
#include "Device.h"
#include "TextureMgr.h"
#include "SceneMgr.h"
#include "ObjMgr.h"
#include "ScrollMgr.h"
#include "FrameMgr.h"
#include "KeyMgr.h"
#include "AstarMgr.h"
