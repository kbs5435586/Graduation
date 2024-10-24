#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_Terrain_Height;
class CTexture;
class CShader;
class CNavigation;
class CFrustum;

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
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CTerrain_Height*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_Terrain_Height*					m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CNavigation*							m_pNaviCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
};

