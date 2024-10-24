#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CObserver;

class CItem;

class CUI_InventorySpace :
    public CUI
{
private:
	CUI_InventorySpace();
	CUI_InventorySpace(const CUI_InventorySpace& rhs);
	virtual ~CUI_InventorySpace() = default;

public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_InventorySpace* Create();
	virtual CGameObject* Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CObserver* m_pObserverCom = nullptr;

	//
	CTexture* m_pItemTextureCom = nullptr;

private:
	POINT MousePos;
	POINT Pos;
	POINT m_Point;

	_float m_fISizeX, m_fISizeY;

	//획득한 아이템
	CItem* cell;
	//아이템 개수
	//아이템의 이미지
};

