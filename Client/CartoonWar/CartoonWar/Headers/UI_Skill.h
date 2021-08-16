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
	void sWorker(const _float& fTimeDelta);
	void sArcher(const _float& fTimeDelta);
	void sMages(const _float& fTimeDelta);

private:
	REP										m_tRep = {};
	XMFLOAT3 pArgTemp{};
	CLASS pClass{};
	
	_bool IsDown{};

private:	//스킬이 다끝날때
	_bool m_SkillActive{};
public:
	_bool& GetSkillActive() { return m_SkillActive; }
	void SetSkillActive(_bool _t) { m_SkillActive = _t; }
private:	//스킬 사용 후 쿨타임 돌게
	_bool StartTime{};
public:		
	_bool& GetSTime() { return StartTime; }
	void SetStime(_bool _t) { StartTime = _t; }
private:	//스킬과 stime모두 끝날때
	_bool m_Active{};
public:
	_bool& GetActive() { return m_Active; }
	void SetActive(_bool _t) { m_Active = _t; }

private:
	_float m_CoolTime_ING{};
	_float m_CoolTime{};
	_float m_MaxCoolTime{}; // 클래스 변경 시 쿨타임 최대치 변경
public:
	_float& GetCoolTime() { return m_CoolTime; }
	void SetCoolTime(_float _t) { m_CoolTime = _t; }

	_float GetMaxCoolTime() { return m_MaxCoolTime; }
	void SetMaxCoolTime(_float _t) { m_MaxCoolTime = _t; }

	//두개짜리 생성 확인용
	//_bool IsTwo{};
	//_bool GetIsTwo() { return IsTwo; }
	//void SetIsTwo(_bool _t) { IsTwo = _t; }
};

