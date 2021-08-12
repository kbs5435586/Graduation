#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;

class CUI_CharTap;
class CUI_Button;
class CUI_ButtonNPC;
class CUI_CharInterface;

class CUI_ClassTap :
    public CUI
{
private:
	CUI_ClassTap();
	CUI_ClassTap(const CUI_ClassTap& rhs);
	virtual ~CUI_ClassTap() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_ClassTap*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
private:
	_bool									m_cansee = {};
	_int									which = {};
	_int									npcnumm = {};
	string									m_strGold{};
	
public:
	_bool GetBool() { return m_cansee; }
	void SetBool(_bool _b) { m_cansee = _b; }
	_int GetWhich() { return which; }
	void SetWhich(_int _w) { which = _w; }
	_int GetNPCNum() {	return npcnumm;	}

};
