#pragma once
#include "Base.h"
class CLight :
	public CBase
{
private:
	CLight(ID3D12Device* pGraphic_Device);
	~CLight() = default;
public:
	HRESULT			Ready_Light(const LIGHT& tLightInfo);
public:
	LIGHT			Get_LightInfo()const { return m_tLight; }

private:
	ID3D12Device*	m_pGraphic_Device = nullptr;
	LIGHT			m_tLight = {};
public:
	static CLight*	Create(ID3D12Device* pGraphic_Device, const LIGHT& tLightInfo);
private:
	virtual void	Free();
};

