#pragma once
#include "GameObject.h"

class CTransform;
class CRenderer;
class CMesh;
class CTexture;
class CShader;
class CFrustum;
class CBoneFire :
	public CGameObject
{
private:
	CBoneFire();
	CBoneFire(const CBoneFire& rhs);
	virtual ~CBoneFire() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_Blur();

private:
	virtual HRESULT							CreateInputLayout();
public:
	static CBoneFire* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pShaderCom_Blur = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CTexture*								m_pTextureCom =  nullptr ;
	CFrustum*								m_pFrustumCom = nullptr;
};


