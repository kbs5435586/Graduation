#pragma once
#include "GameObject.h"
class CMesh;
class CTransform;
class CRenderer;
class CTexture;
class CShader;
class CCollider;

class CThrow_Arrow :
    public CGameObject
{
private:
	CThrow_Arrow();
	CThrow_Arrow(const CThrow_Arrow& rhs);
	virtual ~CThrow_Arrow() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CThrow_Arrow*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CCollider*								m_pColliderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
private:
	_vec3									m_vPos = {};
};

