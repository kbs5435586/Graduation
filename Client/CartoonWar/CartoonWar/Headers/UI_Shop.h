#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;

class CUI_Shop :
    public CUI
{
private:
	CUI_Shop();
	CUI_Shop(const CUI_Shop& rhs);
	virtual ~CUI_Shop() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();	
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_Shop*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();


private:
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	
	list<void*>* lstTemp = {};

	_bool m_cansee = {};
	_bool IsDown = {};
	POINT MousePos;

	_int m_iClass{};
	_int npcNum{};
	static _int tempNum;
};



