#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;
class CUI_Skill :
    public CUI
{
private:
	CUI_Skill();
	CUI_Skill(const CUI_Skill& rhs);
	virtual ~CUI_Skill() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static									CUI_Skill* Create();
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
	_bool IsOnetouch{};
	POINT MousePos{};

	_float m_CoolTime{};
	_float m_MaxCoolTime{};
	XMFLOAT3 pArgTemp{};

	string m_strCoolTime{};
public:
	_bool GetActive() { return m_Active; }
	void SetActive(_bool _t) { m_Active = _t; }

	_bool GetOne() { return IsOnetouch; }
	void SetOne(_bool _t) { IsOnetouch = _t; }

	_float GetCoolTime() { return m_CoolTime; }
	void SetCoolTime(_float _t) { m_CoolTime = _t; }

	_float GetMaxCoolTime() { return m_MaxCoolTime; }
	void SetMaxCoolTime(_float _t) { m_MaxCoolTime = _t; }
};

