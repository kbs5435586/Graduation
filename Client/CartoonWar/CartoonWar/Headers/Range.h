#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CTexture;
class CShader;

class CRange :
    public CGameObject
{
private:
	CRange();
	CRange(const CRange& rhs);
	virtual ~CRange() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();

private:
	virtual HRESULT							CreateInputLayout();
public:
	static CRange* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	//CTexture* m_pTextureCom[3] = { nullptr };
private:
	TEXINFO									m_tTexInfo = {};

private:
	_float									m_range = {};
};

