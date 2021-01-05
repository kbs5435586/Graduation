#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CStatic_Mesh;
class CShader;

class CPicking_Object :
	public CGameObject
{
private:
	CPicking_Object(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPicking_Object(const CPicking_Object& rhs);
	virtual ~CPicking_Object()=default;
public:
	virtual HRESULT			Ready_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void			Render_GameObject();
public:
	static	CPicking_Object* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
private:
	HRESULT					Ready_Component(void* pArg);
	HRESULT					SetUp_ConstantTable(LPD3DXEFFECT pEffect, const _uint& iAttributeID);
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CStatic_Mesh*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

};

