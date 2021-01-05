#pragma once
#include "Base.h"
class CLight;
class CLight_Mgr :
	public CBase
{
	_DECLARE_SINGLETON(CLight_Mgr)
public:
	explicit CLight_Mgr();
	virtual ~CLight_Mgr() = default;
public:
	D3DLIGHT9* Get_LightInfo(const _uint& iIndex = 0);
public:
	HRESULT Add_LightInfo(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightInfo);
private:
	list<CLight*>			m_LightList;
	typedef list<CLight*>	LIGHTLIST;
protected:
	virtual void Free();
};

