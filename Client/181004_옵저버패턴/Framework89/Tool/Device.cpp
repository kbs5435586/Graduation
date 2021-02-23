#include "stdafx.h"
#include "Device.h"

IMPLEMENT_SINGLETON(CDevice)

CDevice::CDevice()
	: m_p3D(nullptr), m_pDevice(nullptr), m_pSprite(nullptr)
{
}


CDevice::~CDevice()
{
	Release();
}

HRESULT CDevice::InitDevice()
{
	// 장치 초기화 과정
	// 1.LPDIRECT3D9 Com객체 생성
	// 2.장치 조사(성능조사, 버텍스 프로세싱 유무 조사)
	//  - 버텍스 프로세싱: 정점처리 + 조명처리
	// 3.LPDIRECT3DDEVICE9 Com객체 생성




	// 1.LPDIRECT3D9 Com객체 생성
	// LPDIRECT3D9은 장치 검증과 LPDIRECT3DDEVICE9 생성을 담당.
	m_p3D = Direct3DCreate9(D3D_SDK_VERSION);

	// 2.장치 조사(성능조사, 버텍스 프로세싱 유무 조사)

	// D3DCAPS9: 현재 장치의 정보를 저장하는 구조체.
	D3DCAPS9 DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// GetDeviceCaps: 장치에 대한 정보를 얻어오는 함수.
	// HAL (Hardware Abstranction Layer, 하드웨어 추상 계층)
	if (FAILED(m_p3D->GetDeviceCaps(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		ERR_MSG(L"GetDeviceCaps Failed!!");
		return E_FAIL;
	}

	// 2-1. 버텍스 프로세싱 유무 조사
	DWORD vp = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 3.LPDIRECT3DDEVICE9 Com객체 생성
	D3DPRESENT_PARAMETERS	d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	SetParameters(d3dpp);

	if (FAILED(m_p3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, vp,
		&d3dpp, &m_pDevice)))
	{
		ERR_MSG(L"Create Device Failed!!");
		return E_FAIL;
	}

	// Sprite Com객체 생성
	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		ERR_MSG(L"Create Sprite Failed!");
		return E_FAIL;
	}

	// Font객체 생성
	D3DXFONT_DESCW	tFontInfo;	// 생성할 폰트 정보
	tFontInfo.Height = 20;
	tFontInfo.Width = 10;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"궁서");

	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont)))
	{
		ERR_MSG(L"Create Font Failed!");
		return E_FAIL;
	}

	return S_OK;
}

void CDevice::Render_Begin()
{
	// 렌더링 과정: 1.후면버퍼를 비운다 -> 2.그리기를 시작 -> 
	// 3.그리기를 종료 -> 4.그리기 결과물을 화면으로 출력한다.

	// 1.후면버퍼를 비운다.
	m_pDevice->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL
		| D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);

	// 2.그리기를 시작
	m_pDevice->BeginScene();
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND); // 알파블렌딩을 허용.
}

void CDevice::Render_End(HWND hWnd)
{
	// 3.그리기를 종료
	m_pSprite->End();
	m_pDevice->EndScene();

	// 4.그리기를 완료하여 화면에 출력한다.
	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);
}

void CDevice::Release()
{
	// 해제 순서 주의!
	if (m_pFont)
		m_pFont->Release();

	if (m_pSprite)
		m_pSprite->Release();

	if (m_pDevice)
		m_pDevice->Release();

	if (m_p3D)
		m_p3D->Release();
}

void CDevice::SetParameters(D3DPRESENT_PARAMETERS& d3dpp)
{
	// 후면버퍼의 가로, 세로 너비.
	d3dpp.BackBufferWidth = WINCX;
	d3dpp.BackBufferHeight = WINCY;

	// 32비트의 픽셀 포맷을 사용.
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

	// 후면버퍼 개수.
	d3dpp.BackBufferCount = 1;

	// 멀티 샘플링을 사용하지 않음.
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	// 멀티 샘플링 품질.
	d3dpp.MultiSampleQuality = 0;

	// 스왑체인 방식을 사용한다.
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// 장치에서 사용할 윈도우
	d3dpp.hDeviceWindow = g_hWnd;

	// 창모드
	d3dpp.Windowed = TRUE;

	// Direcx에서 Z버퍼(깊이)와 스텐실 버퍼를 자동으로 만들고 관리한다.
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// 전체화면시 재생률 (모니터 재생율)
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	// 화면에 뿌릴 간격. (모니터 재생율과 FPS와의 간격)
	// D3DPRESENT_INTERVAL_DEFAULT: 적절한 간격을 Directx가 알아서 조정.
	// D3DPRESENT_INTERVAL_IMMEDIATE: 현재 FPS에 따라 즉시 시연.
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}
