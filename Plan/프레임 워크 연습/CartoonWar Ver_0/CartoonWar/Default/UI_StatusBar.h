#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;

class CUI_StatusBar :
    public CGameObject
{
private:
	CUI_StatusBar();
	CUI_StatusBar(const CUI_StatusBar& rhs);
	virtual ~CUI_StatusBar() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CUI_StatusBar* Create();
	virtual CGameObject* Clone_GameObject(void* pArg) override;

private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	//CTexture* m_pTextureCom = nullptr;
private:
private:
	_float									m_fX, m_fY, m_fZ, m_fSizeX, m_fSizeY, m_fSizeZ;
	POINT MousePos{};
};

