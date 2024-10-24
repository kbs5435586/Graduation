#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CMesh;
class CHatch :
	public CGameObject
{
private:
	CHatch();
	CHatch(const CHatch& rhs);
	virtual ~CHatch() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	HRESULT									Ready_Component(const _tchar* pComTag);
	virtual HRESULT							CreateInputLayout();
public:
	static CHatch* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx=0);
private:
	virtual void							Free();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CMesh* m_pMeshCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CFrustum* m_pFrustumCom = nullptr;
};
