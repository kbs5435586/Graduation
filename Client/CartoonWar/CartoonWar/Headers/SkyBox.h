#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CBuffer_CubeTex;
class CSkyBox :
    public CGameObject
{
private:
    CSkyBox();
    CSkyBox(const CSkyBox& rhs);
    virtual ~CSkyBox() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
	HRESULT									CreateConstantBuffer();
public:
	static CSkyBox*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CBuffer_CubeTex*						m_pBufferCom = nullptr;
};

