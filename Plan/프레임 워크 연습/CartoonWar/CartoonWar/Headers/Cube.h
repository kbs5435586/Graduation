#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CBuffer_CubeCol;
class CCollider;

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
	bool f = true;
	bool getbool() { return f; };
	
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CTransform*								m_pTransformComT = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_CubeCol*						m_pBufferCom = nullptr;
	CBuffer_CubeCol*						m_pBufferComT = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CCollider*								m_pColliderCom[3] = { nullptr };

	CShader*								m_pShaderComT = nullptr;

	_vec3 m;
};

