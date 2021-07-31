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
public:
	CUI_Shop();
	CUI_Shop(const CUI_Shop& rhs);
	virtual ~CUI_Shop() = default;
public:
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta, _bool b[], int idx);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture);

	HRESULT									Ready_Component();

	void setSize(_float x, _float y) { m_fSizeX = x; m_fSizeY = y; };
	void setPos(_float x, _float y) { m_fX = x; m_fY = y; };
	void setObs(CObserver* _obs) { m_pObserverCom = _obs; }

	CObserver* m_pObserverCom = {};
	list<void*>* lstTemp = {};


	_bool m_cansee = {};
	_bool IsDown = {};
	POINT MousePos;

	_int m_iClass{};
	_int npcNum{};
	static _int tempNum;
};



