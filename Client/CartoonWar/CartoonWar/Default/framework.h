// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <array>

using namespace std;
#define DIRECTINPUT_VERSION 0x0800
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <Mmsystem.h>
#include <dxgi1_4.h>
#include <dinput.h>
#include <wincodec.h>
#include <time.h>
#include <process.h>
#include <wrl.h>
#include <fstream>
#include <string>
#include <wrl/client.h>

#include "SimpleMath.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "DInput8.lib")
#pragma comment(lib, "DXGuid.lib")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

using namespace DirectX::SimpleMath;

#include "d3dx12.h"
#include "Define.h"
#include "Extern.h"
#include "Function.h"
#include "Struct.h"
#include "Typedef.h"
#include "Funtor.h"


#include "Device.h"
#include "Input.h"
#include "Camera_Manager.h"