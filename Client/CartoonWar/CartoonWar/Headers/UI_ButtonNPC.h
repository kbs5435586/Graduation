#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;
class CUI_ButtonNPC :
    public CUI
{
public:
	CUI_ButtonNPC();
	CUI_ButtonNPC(const CUI_ButtonNPC& rhs);
	virtual ~CUI_ButtonNPC() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_ButtonNPC* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();

private:
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;


	void setSize(_float x, _float y) { m_fSizeX = x; m_fSizeY = y; };
	void setPos(_float x, _float y) { m_fX = x; m_fY = y; };
	
	
	list<void*>* lstTemp = {};


	_bool m_cansee = {};
	_bool IsDown = {};
	_bool UnitActive{};
	POINT MousePos = {};

	_uint m_ButtonNow{};
	
	_uint npcnumm{};
	_uint now{};
	
	
	

	static _int tempNum;
};

