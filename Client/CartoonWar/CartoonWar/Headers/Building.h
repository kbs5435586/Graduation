#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CMesh;
class CBuilding :
    public CGameObject
{
private:
	CBuilding();
	CBuilding(const CBuilding& rhs);
	virtual ~CBuilding() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_Blur();
	virtual void							Render_OutLine();
private:
	HRESULT									Ready_Component(const _tchar* pComTag);
	virtual HRESULT							CreateInputLayout();
public:
	static CBuilding*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx=0);
private:
	virtual void							Free();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CShader*								m_pShaderCom_OutLine = nullptr;
	CShader*								m_pShaderCom_Blur = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
private:
	vector<CTexture*>						m_vecTexture;
};

