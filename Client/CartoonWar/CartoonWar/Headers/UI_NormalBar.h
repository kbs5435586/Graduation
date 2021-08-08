#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;

class CUI_NormalBar :
	public CUI
{
private:
	CUI_NormalBar();
	CUI_NormalBar(const CUI_NormalBar& rhs);
	virtual ~CUI_NormalBar() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_NormalBar* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
private:
	REP										m_tRep = {};
	_bool									m_IsTemp = false;
	_bool									m_IsRed = false;
	_int									m_iMaxTexCnt = 11;
	_float									m_fCurCnt = 0.f;
};

