#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CMesh;
class CFrustum;
class CTexture;
class CFlag :
	public CGameObject
{
private:
	CFlag();
	CFlag(const CFlag& rhs);
	virtual ~CFlag() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_Blur();			
public:
	void									SetTeam(TEAM eCurTeam);
private:
	HRESULT									Ready_Component();
	virtual HRESULT							CreateInputLayout();
public:
	static CFlag*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0);
private:
	virtual void							Free();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CShader*								m_pShaderCom_Blur = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
public:
	_uint									m_iCnt = 0;
};

