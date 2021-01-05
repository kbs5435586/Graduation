#pragma once
#include "Component.h"
class CTransform;
class CMyFont :
	public CComponent
{
private:
	CMyFont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMyFont(const CMyFont& rhs);
	virtual ~CMyFont()=default;
public:
	HRESULT			Ready_Font();
	HRESULT			SetUp_3DFont(const char* pStr, CTransform* pTransform, int r, int g, int b);
public:
	static CMyFont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
private:
	D3DXVECTOR3 GetScreenPos(_vec3& vpos);
private:
	LPD3DXSPRITE		m_pSprite=nullptr;
	LPD3DXFONT			m_pFont=nullptr;
	RECT rt = {};


};

