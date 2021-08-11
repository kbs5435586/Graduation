#pragma once
#include "UI.h"

class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;


class CUI_Button :
    public CUI
{
private:
	CUI_Button();
	CUI_Button(const CUI_Button& rhs);
	virtual ~CUI_Button() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_Button* Create();
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
	void setObs(CObserver* _obs);

	//CObserver* m_pObserverCom = {};

	_bool m_cansee = {};
	_bool IsDown = {};
	POINT MousePos;

	CLASS m_ButtonClass{};
	//_int whichnum{};

	_vec2 vTemp{};
	static _int tempNum;
};
