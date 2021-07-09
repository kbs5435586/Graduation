#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CBuffer_RectTex;
class CTexture;

class CMyRect :
    public CGameObject
{
private:
    CMyRect();
    CMyRect(const CMyRect& rhs);
    virtual ~CMyRect() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CMyRect*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx=0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component(_uint iNum = 0);
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = {nullptr};
	CTexture*								m_pTextureCom = nullptr;
private:
	REP										m_tRep = {};
	TEAM									m_eCurTeam = TEAM::TEAM_END;
	TEAM									m_ePreTeam = TEAM::TEAM_END;
private:
	_bool									m_IsTemp = false;
	_bool									m_IsChange = false;
	_bool									m_IsFix = false;
	_float									m_fTempTime = 0.f;
	_float									m_fDeltaTime = 0.f;
private:
	_uint									m_iNum = 0;
	
};

