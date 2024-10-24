#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CTexture;
class CShader;
class CFrustum;
class CFire :
	public CGameObject
{
private:
	CFire();
	CFire(const CFire& rhs);
	virtual ~CFire() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static	CFire*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();

private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom[3] = { nullptr };
	CFrustum*								m_pFrustumCom = nullptr;
private:
	TEXINFO									m_tTexInfo = {};

};

