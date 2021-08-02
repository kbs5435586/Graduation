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
class CUI_Shop;

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
	static CUI_ClassTap* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	//CTransform* m_pTransformCom = nullptr;
	//CTransform* m_pCharTapsTransformCom[5] = {};
	//CTransform* m_pClassTapsTransformCom[5] = {};

	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CShader* m_pBlendShaderCom = nullptr;
	CShader* m_pInvenShaderCom = nullptr;
	CShader* m_pCompute_ShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pButtonTextureCom = nullptr;
	CTexture* m_pNPCTextureCom = nullptr;
	CTexture* m_pIconTextureCom = nullptr;
	CTexture* m_pPressedNPCTextureCom = nullptr;

private:
	REP										m_tRep = {};
	_bool									m_IsTemp = false;
	
	_bool									m_tapActive = false;
	_bool									m_IsTap[5] = {};
	
	
	CUI_Button*								m_button[9] = {};
	CUI_ButtonNPC*							m_buttonNPC[15] = {};
	CUI_CharInterface*						m_charInter = nullptr;
	CUI_Shop* m_shop = nullptr;

	//CObserver* m_pObserverCom = {};

	POINT MousePos = {};
	
	_int m_meshnum[5] = {};
	
	_int* pwhich = {};
	_int npcnumm{};

	string m_strGold{};

	_bool									m_cansee = false;
	_int which = {};
public:
	_bool GetBool() { return m_cansee; }
	void SetBool(_bool _b) { m_cansee = _b; }
	_int GetWhich() { return which; }
	void SetWhich(_int _w) { which = _w; }

};
