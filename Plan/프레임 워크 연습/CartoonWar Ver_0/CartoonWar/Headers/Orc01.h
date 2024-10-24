#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CStatic_Mesh;
class CShader;
class CTexture;
class CFrustum;
class CNavigation;

class COrc01 :
    public CGameObject
{
private:
	COrc01();
	COrc01(const COrc01& rhs);
    virtual ~COrc01() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	HRESULT									Ready_Component();
	HRESULT									Ready_Light(MAINPASS& tPass);
private:
	virtual HRESULT							CreateInputLayout();
public:
    static COrc01*							Create();
    virtual CGameObject*					Clone_GameObject(void* pArg);
private:
    virtual void							Free();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CStatic_Mesh*							m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CNavigation*							m_pNaviCom = nullptr;

};

