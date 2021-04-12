#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CAnimator;
class CNavigation;
class CTestAnimMesh :
    public CGameObject
{
private:
	CTestAnimMesh();
	CTestAnimMesh(const CTestAnimMesh& rhs);
	virtual ~CTestAnimMesh() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CTestAnimMesh*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint & iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pComputeShaderCom = nullptr;
	CAnimator*								m_pAnimCom = nullptr;
	CNavigation*							m_pNaviCom = nullptr;
private:
	vector<CTexture*>						m_vecTexture;
	_bool									m_IsAnim = false;
};

