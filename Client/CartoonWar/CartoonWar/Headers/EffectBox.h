#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CFrustum;
class CTexture;
class CPlayer;
class CEffectBox :
    public CGameObject
{
private:
    CEffectBox();
    CEffectBox(const CEffectBox& rhs);
    virtual ~CEffectBox() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CEffectBox*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
public:
	CPlayer*&								GetOwnPlayer() { return m_pOwnPlayer; }
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
	CTexture*								m_pTextureCom[3] = { nullptr };
private:
	CPlayer*								m_pOwnPlayer = nullptr;
private:
	POSSIZE									m_tPosizeInfo = {};
private:
		TEXINFO									m_tTexInfo = {};
};

