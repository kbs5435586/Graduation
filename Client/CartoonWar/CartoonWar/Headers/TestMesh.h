#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;

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
private:
	HRESULT									Ready_Component();
	virtual HRESULT							CreateInputLayout();
public:
	static CTestMesh*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg);
private:
	virtual void							Free();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
};
