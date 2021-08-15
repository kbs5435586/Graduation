#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CTexture;
class CShader;
class CFrustum;

class CUI_OnHead_Gage :
    public CUI
{
private:
	CUI_OnHead_Gage();
	CUI_OnHead_Gage(const CUI_OnHead_Gage& rhs);
	virtual ~CUI_OnHead_Gage() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_OnHead_Gage*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
public:
	void									SetPosition(_vec3 vPos);
	void									ResetGage();
	void									FixGage();
public:
	_bool&									GetIsReset(){return m_IsReset;}
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
	ORDER									m_tOrder = {};
private:
	_bool									m_IsReset = false;
	_bool									m_IsFix_Gage = false;
	_float									m_fTimeDelta = 0.f;
	_uint									m_iTeam = 0;
	_uint									m_iTeamCnt = 0;
};

