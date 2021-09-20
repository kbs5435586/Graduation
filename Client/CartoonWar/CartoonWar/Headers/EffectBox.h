#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_CubeCol;;
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
	void									Create_Particle(const _vec3& vPoistion);
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_CubeCol*							m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pShaderCom_Compute = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
	CTexture*								m_pTextureCom[3] = { nullptr };
private:
	CPlayer*								m_pOwnPlayer = nullptr;
	CGameObject*							m_pParticle = nullptr;
private:
	_float									m_fLifeTime = 0.f;
	_float									m_fAccTime = 0.f;
};

