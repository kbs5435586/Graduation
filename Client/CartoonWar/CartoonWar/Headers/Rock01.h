#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CStatic_Mesh;
class CShader;
class CRock01 :
    public CGameObject
{
private:
    CRock01();
    CRock01(const CRock01& rhs);
    virtual ~CRock01() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CRock01*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CStatic_Mesh*							m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;

};

