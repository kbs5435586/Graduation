#pragma once
#include "Base.h"
class CDevice :
	public CBase
{
	_DECLARE_SINGLETON(CDevice)
public:
	CDevice();
	virtual ~CDevice()=default;
public:
	LPDIRECT3DDEVICE9 Get_Graphic_Device() const {return m_pDevice;	}
public:
	HRESULT Ready_Graphic_Device(WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY, LPDIRECT3DDEVICE9* ppGraphic_Device);

private:
	LPDIRECT3D9					m_pSDK=nullptr;
	LPDIRECT3DDEVICE9			m_pDevice=nullptr;
private:
	void SetParameters(D3DPRESENT_PARAMETERS& d3dpp, WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY);
public:
	virtual void Free(void);
};

