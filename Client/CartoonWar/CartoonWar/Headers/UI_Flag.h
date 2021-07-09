#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;
class CUI_Flag :
    public CUI
{
private:
	CUI_Flag();
	CUI_Flag(const CUI_Flag& rhs);
	virtual ~CUI_Flag() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static									CUI_Flag* Create();
	virtual									CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CShader* m_pBlendShaderCom = nullptr; 
	CShader* m_pCompute_ShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

private:
	REP										m_tRep = {};

	CObserver* m_pObserverCom = {};

	_bool m_Active{};
	_bool IsDown{};
	POINT MousePos{};

	_float m_CoolTime{};
};

