#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CBuffer_CubeCol;
class CCollider;
class CObserver;

class CCube :
    public CGameObject
{
private:
    CCube();
    CCube(const CCube& rhs);
    virtual ~CCube() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CCube*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_CubeCol*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CCollider*								m_pColliderCom[3] = { nullptr };
	CObserver*								m_pObserverCom = nullptr;
};

