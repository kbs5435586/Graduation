#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_Point;
class CShader;
class CParticle;
class CTexture;

class CParticle_Default :
    public CGameObject
{
private:
    CParticle_Default();
    CParticle_Default(const CParticle_Default& rhs);
    virtual ~CParticle_Default() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CParticle_Default*				Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_Point*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom[2] = {nullptr};
	CTexture*								m_pTextureCom = nullptr;
	CTexture*								m_pTextureCom_Noise = nullptr;
	CParticle*								m_pParticleCom = nullptr;
};

