#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_Sphere;
class CShader;

class CCircle :
    public CGameObject
{
private:
    CCircle();
    CCircle(const CCircle& rhs);
    virtual ~CCircle() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CCircle*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_Sphere*							m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;

};

