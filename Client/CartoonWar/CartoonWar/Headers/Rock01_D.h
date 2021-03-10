#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CRock01_D :
    public CGameObject
{
private:
    CRock01_D();
    CRock01_D(const CRock01_D& rhs);
    virtual ~CRock01_D() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CRock01_D*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	//CStatic_Mesh*							m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
};

