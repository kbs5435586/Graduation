#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CTexture;
class CShader;

class CUI_OnHeadBack :
	public CUI
{
private:
	CUI_OnHeadBack();
	CUI_OnHeadBack(const CUI_OnHeadBack& rhs);
	virtual ~CUI_OnHeadBack() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_OnHeadBack* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
public:
	void									SetPosition(_vec3 vPos, CLASS eClass);
	void									SetInfo(INFO tInfo);
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
private:
	_vec3									m_vPos = {};
	INFO									m_tUnitInfo = {};
};

