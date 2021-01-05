#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CDynamic_Mesh;
class CShader;
class CPicking;
class COnHeadFont;

class CBeatle :
	public CGameObject
{
private:
	CBeatle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBeatle(const CBeatle& rhs);
	virtual ~CBeatle() = default;
public:
	virtual HRESULT			Ready_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void			Render_GameObject();
public:
	static	CBeatle*		Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
private:
	HRESULT					Ready_Component();
	HRESULT					SetUp_ConstantTable(LPD3DXEFFECT pEffect, const D3DXMESHCONTAINER_DERIVED* pMeshContainer, const _uint& iAttributeID);
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CDynamic_Mesh*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CPicking*				m_pPickingCom = nullptr;
private:
	COnHeadFont*			m_pOnHeadFont = nullptr;
};

