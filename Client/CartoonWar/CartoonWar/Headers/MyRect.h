#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CBuffer_RectTex;
class CTexture;

class CMyRect :
    public CGameObject
{
private:
    CMyRect();
    CMyRect(const CMyRect& rhs);
    virtual ~CMyRect() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CMyRect*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint& iIdx) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom[2] = {nullptr};
	CTexture*								m_pTextureCom = nullptr;

};

