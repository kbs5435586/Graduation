#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CMesh;
class CTestMesh :
    public CGameObject
{
private:
	CTestMesh();
	CTestMesh(const CTestMesh& rhs);
	virtual ~CTestMesh() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_Blur();
private:
	HRESULT									Ready_Component();
	virtual HRESULT							CreateInputLayout();
public:
	static CTestMesh*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx=0);
private:
	virtual void							Free();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
	CCollider*								m_pCollider_Obb = nullptr;
	CCollider*								m_pCollider_AABB = nullptr;
};

