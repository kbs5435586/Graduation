#include "stdafx.h"
#include "Device.h"

_IMPLEMENT_SINGLETON(CDevice)
CDevice::CDevice()
{
}


HRESULT CDevice::Ready_Graphic_Device(WINMODE Mode, HWND hWnd, const WORD & wSizeX, const WORD & wSizeY, LPDIRECT3DDEVICE9 * ppGraphic_Device)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9		devicecaps;
	ZeroMemory(&devicecaps, sizeof(D3DCAPS9));

	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &devicecaps)))
	{
		_MSG_BOX("GetDeviceCaps Failed");
		return E_FAIL;
	}

	DWORD		vp;

	if (devicecaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	vp |= D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS	d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	SetParameters(d3dpp, Mode, hWnd, wSizeX, wSizeY);

	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL
		, hWnd, vp, &d3dpp, &m_pDevice)))
	{
		_MSG_BOX("CreateDevice Failed");
		return E_FAIL;
	}

	if (nullptr != ppGraphic_Device)
	{
		*ppGraphic_Device = m_pDevice;
		m_pDevice->AddRef();
	}

	return S_OK;
}

void CDevice::SetParameters(D3DPRESENT_PARAMETERS & d3dpp, WINMODE Mode, HWND hWnd, const WORD & wSizeX, const WORD & wSizeY)
{
	d3dpp.BackBufferWidth = wSizeX;
	d3dpp.BackBufferHeight = wSizeY;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

	d3dpp.MultiSampleQuality = 0;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.EnableAutoDepthStencil = TRUE;

	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = Mode;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

void CDevice::Free(void)
{
	_ulong		dwRefCnt = 0;

	if (dwRefCnt = Safe_Release(m_pDevice))
		_MSG_BOX("CGraphic_Device Release Failed");

	if (dwRefCnt = Safe_Release(m_pSDK))
		_MSG_BOX("SDK Release Failed");

}
