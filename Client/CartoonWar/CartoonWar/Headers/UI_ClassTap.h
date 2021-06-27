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
	CShader* m_pInvenShaderCom = nullptr;
	CShader* m_pCompute_ShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

private:
	REP										m_tRep = {};
	_bool									m_IsTemp = false;
	_bool									m_cansee = false;
	_bool									m_IsTap[5] = {};
	//_float									m_fCharTapSizeX, m_fCharTapSizeY{};
	//_float									m_fCharTapfX[5],	m_fCharTapfY[5]{};
	//_float									m_fClassTapSizeX,	m_fClassTapSizeY{};
	//_float									m_fClassTapX,		m_fClassTapY{};
	CUI_CharTap*							a = nullptr;
	CUI_Button*								m_button = nullptr;
	CUI_CharInterface*						m_charInter = nullptr;

	POINT MousePos = {};
	_bool IsSwitch = {};
};

class CUI_CharTap :public CUI
{
public:
	CUI_CharTap();
	CUI_CharTap(const CUI_ClassTap& rhs);
	virtual ~CUI_CharTap() = default;
public:
	//virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta, _bool b[], int idx);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture);

	void setSize(_float x, _float y);
	void setPos(_float x, _float y);
	_float getSizeX();
	_float getSizeY();
	_float getX();
	_float getY();
	bool getActive();
private:
private:
	POINT MousePos;
	_bool isActive;

};