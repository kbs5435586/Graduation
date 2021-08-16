#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;

class CUI_End :
	public CUI
{
private:
	CUI_End();
	CUI_End(const CUI_End& rhs);
	virtual ~CUI_End() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_End*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom[2] = { nullptr };
private:
	_bool									m_IsWin = true;
private:
	_uint									m_iWinTextureMaxCnt = 0;
	_uint									m_iWinTextureCnt = 0;
	_float									m_fWinTimeDelta = 0.f;
private:
	_uint									m_iOverTextureMaxCnt = 0;
	_uint									m_iOverTextureCnt = 0;
	_float									m_fOverTimeDelta = 0.f;
};

