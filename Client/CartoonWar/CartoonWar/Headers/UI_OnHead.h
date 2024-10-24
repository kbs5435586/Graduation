#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CTexture;
class CShader;
class CFrustum;

class CUI_OnHead :
    public CUI
{
private:
    CUI_OnHead();
    CUI_OnHead(const CUI_OnHead& rhs);
    virtual ~CUI_OnHead() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_OnHead*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
public:
	void									SetPosition(_vec3 vPos, CLASS eClass);
	void									SetPosition(_vec3 vPos, ANIMALS eAnimals);
	void									SetInfo(INFO tInfo);
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
private:
	_vec3									m_vPos = {};
	_vec3									m_vBarPos = {};
	_vec3									m_vSize = {};
	INFO									m_tUnitInfo = {};
	ORDER									m_tOrder = {};
private:
	_bool									m_IsCreate = false;
};

