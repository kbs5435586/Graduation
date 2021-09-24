#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_Point;
class CShader;
class CParticle;
class CTexture;


class CParticle_FireBall :
	public CGameObject
{
private:
	CParticle_FireBall();
	CParticle_FireBall(const CParticle_FireBall& rhs);
	virtual ~CParticle_FireBall() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CParticle_FireBall* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
public:
	void									Set_Particle(PARTICLESET tParticleSet);

private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_Point* m_pBufferCom = nullptr;
	CShader* m_pShaderCom[2] = { nullptr };
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pTextureCom_Noise = nullptr;
	CParticle* m_pParticleCom = nullptr;
private:
	_float									m_fMaxLifeTime = 0.f;
	_float									m_fLifeTime = 0.f;
private:
	_vec3									m_vPos = {};
public:
	_vec3&									GetPos(){return m_vPos;}
};

