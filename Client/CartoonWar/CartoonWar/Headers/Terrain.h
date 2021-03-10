#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CBuffer_Terrain;
class CTexture;
class CNavigation;

class CTerrain :
    public CGameObject
{
private:
    CTerrain();
    CTerrain(const CTerrain& rhs);
    virtual ~CTerrain() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CTerrain*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_Terrain*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CNavigation*							m_pNaviCom = nullptr;
};

