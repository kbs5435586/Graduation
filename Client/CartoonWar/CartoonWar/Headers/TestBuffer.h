#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CBuffer_RectTex;
class CTestBuffer :
	public CGameObject
{
private:
	CTestBuffer();
	CTestBuffer(const CTestBuffer& rhs);
	virtual ~CTestBuffer() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_PostEffect();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CTestBuffer* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CShader* m_pShaderCom_PostEffect = nullptr;
	CTexture* m_pTextureCom = nullptr;
private:
	TEXINFO									m_tTexInfo = {};
private:
	_float									m_fPostEffectTime = 0.f;
};


