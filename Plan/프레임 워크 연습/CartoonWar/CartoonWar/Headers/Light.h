#pragma once
#include "Base.h"
class CShader;
class CLight :
	public CBase
{
private:
	CLight();
	~CLight() = default;
public:
	HRESULT						Ready_Light(const LIGHT& tLightInfo);
public:
	LIGHT&						Get_LightInfo() { return m_tLight; }
private:
	LIGHT						m_tLight = {};
public:
	static CLight*				Create(const LIGHT& tLightInfo);
private:
	virtual void				Free();
};

