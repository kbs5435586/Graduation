#pragma once
#include "Base.h"
class CLight :
	public CBase
{
private:
	CLight();
	~CLight() = default;
public:
	HRESULT			Ready_Light(const LIGHT& tLightInfo);
public:
	LIGHT			Get_LightInfo()const { return m_tLight; }

	LIGHT			m_tLight = {};
public:
	static CLight*	Create(const LIGHT& tLightInfo);
private:
	virtual void	Free();
};

