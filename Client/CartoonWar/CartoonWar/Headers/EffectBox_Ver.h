#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CFrustum;
class CTexture;
class CPlayer;
class CEffectBox_Ver :
	public CGameObject
{
private:
	CEffectBox_Ver();
	CEffectBox_Ver(const CEffectBox_Ver& rhs);
	virtual ~CEffectBox_Ver() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	CPlayer*& GetOwnPlayer() { return m_pOwnPlayer; }
private:
	CPlayer* m_pOwnPlayer = nullptr;
public:
	static CEffectBox_Ver* Create();
	virtual CGameObject* Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CTransform* m_pTransformCom_Ver = nullptr;

	CRenderer* m_pRendererCom = nullptr;

	CBuffer_RectTex* m_pBufferCom = nullptr;
	CBuffer_RectTex* m_pBufferCom_Ver = nullptr;

	CShader* m_pShaderCom = nullptr;
	CFrustum* m_pFrustumCom = nullptr;
	CTexture* m_pTextureCom[3] = { nullptr };
private:
	POSSIZE									m_tPosizeInfo = {};
private:
	TEXINFO								m_tTexInfo = {};
};

