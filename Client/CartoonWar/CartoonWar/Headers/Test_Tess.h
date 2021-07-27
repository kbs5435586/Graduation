#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CBuffer_RectTex;
class CTest_Tess :
	public CGameObject
{
private:
	CTest_Tess();
	CTest_Tess(const CTest_Tess& rhs);
	virtual ~CTest_Tess() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CTest_Tess*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
};

