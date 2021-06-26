#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;

class CUI_CharInterface :
    public CUI
{
public:
	CUI_CharInterface();
	CUI_CharInterface(const CUI_CharInterface& rhs);
	virtual ~CUI_CharInterface() = default;
public:
	//virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta, _bool b[], int idx);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject(CShader * shader, CBuffer_RectTex * buffer, CTexture * texture);
};

