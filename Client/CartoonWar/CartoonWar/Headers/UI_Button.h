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
public:
	CUI_Button();
	CUI_Button(const CUI_Button& rhs);
	virtual ~CUI_Button() = default;
public:
	//virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta, _bool b[], int idx);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject(CShader * shader, CBuffer_RectTex * buffer, CTexture * texture);

	//void setSize(_float x, _float y);
	//void setPos(_float x, _float y);
	//_float getSizeX();
	//_float getSizeY();
	//_float getX();
	//_float getY();
	//bool getActive();



	POINT MousePos;
};

