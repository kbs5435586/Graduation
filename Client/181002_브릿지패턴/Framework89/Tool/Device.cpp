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
	// ��ġ �ʱ�ȭ ����
	// 1.LPDIRECT3D9 Com��ü ����
	// 2.��ġ ����(��������, ���ؽ� ���μ��� ���� ����)
	//  - ���ؽ� ���μ���: ����ó�� + ����ó��
	// 3.LPDIRECT3DDEVICE9 Com��ü ����




	// 1.LPDIRECT3D9 Com��ü ����
	// LPDIRECT3D9�� ��ġ ������ LPDIRECT3DDEVICE9 ������ ���.
	m_p3D = Direct3DCreate9(D3D_SDK_VERSION);

	// 2.��ġ ����(��������, ���ؽ� ���μ��� ���� ����)

	// D3DCAPS9: ���� ��ġ�� ������ �����ϴ� ����ü.
	D3DCAPS9 DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// GetDeviceCaps: ��ġ�� ���� ������ ������ �Լ�.
	// HAL (Hardware Abstranction Layer, �ϵ���� �߻� ����)
	if (FAILED(m_p3D->GetDeviceCaps(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		ERR_MSG(L"GetDeviceCaps Failed!!");
		return E_FAIL;
	}

	// 2-1. ���ؽ� ���μ��� ���� ����
	DWORD vp = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 3.LPDIRECT3DDEVICE9 Com��ü ����
	D3DPRESENT_PARAMETERS	d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	SetParameters(d3dpp);

	if (FAILED(m_p3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, vp,
		&d3dpp, &m_pDevice)))
	{
		ERR_MSG(L"Create Device Failed!!");
		return E_FAIL;
	}

	// Sprite Com��ü ����
	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		ERR_MSG(L"Create Sprite Failed!");
		return E_FAIL;
	}

	// Font��ü ����
	D3DXFONT_DESCW	tFontInfo;	// ������ ��Ʈ ����
	tFontInfo.Height = 20;
	tFontInfo.Width = 10;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"�ü�");

	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont)))
	{
		ERR_MSG(L"Create Font Failed!");
		return E_FAIL;
	}

	return S_OK;
}

void CDevice::Render_Begin()
{
	// ������ ����: 1.�ĸ���۸� ���� -> 2.�׸��⸦ ���� -> 
	// 3.�׸��⸦ ���� -> 4.�׸��� ������� ȭ������ ����Ѵ�.

	// 1.�ĸ���۸� ����.
	m_pDevice->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL
		| D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);

	// 2.�׸��⸦ ����
	m_pDevice->BeginScene();
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND); // ���ĺ����� ���.
}

void CDevice::Render_End(HWND hWnd)
{
	// 3.�׸��⸦ ����
	m_pSprite->End();
	m_pDevice->EndScene();

	// 4.�׸��⸦ �Ϸ��Ͽ� ȭ�鿡 ����Ѵ�.
	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);
}

void CDevice::Release()
{
	// ���� ���� ����!
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
	// �ĸ������ ����, ���� �ʺ�.
	d3dpp.BackBufferWidth = WINCX;
	d3dpp.BackBufferHeight = WINCY;

	// 32��Ʈ�� �ȼ� ������ ���.
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

	// �ĸ���� ����.
	d3dpp.BackBufferCount = 1;

	// ��Ƽ ���ø��� ������� ����.
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	// ��Ƽ ���ø� ǰ��.
	d3dpp.MultiSampleQuality = 0;

	// ����ü�� ����� ����Ѵ�.
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// ��ġ���� ����� ������
	d3dpp.hDeviceWindow = g_hWnd;

	// â���
	d3dpp.Windowed = TRUE;

	// Direcx���� Z����(����)�� ���ٽ� ���۸� �ڵ����� ����� �����Ѵ�.
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// ��üȭ��� ����� (����� �����)
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	// ȭ�鿡 �Ѹ� ����. (����� ������� FPS���� ����)
	// D3DPRESENT_INTERVAL_DEFAULT: ������ ������ Directx�� �˾Ƽ� ����.
	// D3DPRESENT_INTERVAL_IMMEDIATE: ���� FPS�� ���� ��� �ÿ�.
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}
