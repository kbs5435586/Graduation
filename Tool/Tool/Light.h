#pragma once
#include "Base.h"
class CLight :
	public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLight() = default;
public:
	D3DLIGHT9* Get_LightInfo()  {
		return &m_LightInfo;
	}
public:
	HRESULT Ready_Light(const D3DLIGHT9& LightInfo);
private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	D3DLIGHT9			m_LightInfo;
public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightInfo);
	virtual void Free();
};

