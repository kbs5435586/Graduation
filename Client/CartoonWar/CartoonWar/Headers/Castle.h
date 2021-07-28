#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CMesh;
class CTexture;
class CCastle :
	public CGameObject
{
private:
	CCastle();
	CCastle(const CCastle& rhs);
	virtual ~CCastle() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_Blur();
private:
	HRESULT									Ready_Component(const _tchar* pComTag);
	virtual HRESULT							CreateInputLayout();
public:
	static CCastle* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0);
private:
	virtual void							Free();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CShader* m_pShaderCom_Shadow = nullptr;
	CShader* m_pShaderCom_Blur = nullptr;
	CMesh* m_pMeshCom = nullptr;
	CFrustum* m_pFrustumCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

private:
	vector<CTexture*>						m_vecTexture;
};

