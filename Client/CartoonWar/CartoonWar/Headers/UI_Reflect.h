#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CUI_Reflect :
    public CUI
{
private:
	CUI_Reflect();
	CUI_Reflect(const CUI_Reflect& rhs);
	virtual ~CUI_Reflect() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_Reflect*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
private:
	//unique_ptr<DirectX::SpriteFont>			m_pResource;
};

