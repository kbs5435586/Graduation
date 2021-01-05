#pragma once
#include "Component.h"
class CShader :
	public CComponent
{
private:
	CShader(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShader(const CShader& rhs);
	virtual ~CShader()=default;
public:
	LPD3DXEFFECT		GetEffectHandle()const { return m_pEffect; }
public:
	HRESULT				Ready_Shader(const _tchar* pFilePath);
public:
	static CShader*		Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath);
	virtual CComponent*	Clone_Component(void* pArg);
	virtual void		Free();
private:
	LPD3DXEFFECT		m_pEffect = nullptr;
	LPD3DXBUFFER		m_pBuffer = nullptr;
	
};

