#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CAnimator;
class CRock03 :
    public CGameObject
{
private:
	CRock03();
	CRock03(const CRock03& rhs);
	virtual ~CRock03() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CRock03*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
};
