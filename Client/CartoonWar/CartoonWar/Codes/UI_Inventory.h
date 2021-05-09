#pragma once
#include "UI.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;

class CUI_Diffuse;
class CUI_Loading;
class CItem;

typedef struct item {
	int HP, MP;
	string name;

}ITEM;


class CUI_Inventory :
	public CUI
{
private:
	CUI_Inventory();
	CUI_Inventory(const CUI_Inventory& rhs);
	virtual ~CUI_Inventory() = default;

public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_Inventory* Create();
	virtual CGameObject* Clone_GameObject(void* pArg, const _uint& iIdx) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CShader* m_pShaderComT = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pITextureCom = nullptr;
private:
	POINT MousePos;
	POINT Pos;
	POINT IPos[4];
	vector<ITEM> vList;
	bool canSee;
};


