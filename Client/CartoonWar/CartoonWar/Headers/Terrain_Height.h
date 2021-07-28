#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_Terrain_Height;
class CTexture;
class CShader;
class CNavigation;
class CFrustum;
class CPicking;

class CTerrain_Height :
    public CGameObject
{
private:
    CTerrain_Height();
    CTerrain_Height(const CTerrain_Height& rhs);
    virtual ~CTerrain_Height() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CTerrain_Height*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_Terrain_Height*					m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pBrushTextureCom = nullptr;
	CNavigation*							m_pNaviCom = nullptr;
	CPicking*								m_pPickingCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
private:
	CTexture*								m_pTextureCom_Grass_Mix = nullptr;
	CTexture*								m_pTextureCom_Ground = nullptr;
	CTexture*								m_pTextureCom_Fillter = nullptr;
private:
	BRUSHINFO								m_tBrush = {};
public:
	BRUSHINFO&								GetBrushINFO() { return m_tBrush; }
};

