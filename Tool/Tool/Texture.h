#pragma once
#include "Component.h"
class CTexture :
	public CComponent
{
private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;
public:
	HRESULT Ready_Texture(TEXTURE_TYPE eType, const _tchar* pFilePath, const _uint& iNumTexture);
	HRESULT SetUp_OnGraphicDev(const _uint& iIndex = 0);
	HRESULT	SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _uint& iIdx=0);
private:
	vector<LPDIRECT3DTEXTURE9>			m_vecTexture;
	typedef vector<LPDIRECT3DTEXTURE9>	VECTEXTURE;
public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURE_TYPE eType, const _tchar* pFilePath, const _uint& iNumTexture = 1);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();

};

