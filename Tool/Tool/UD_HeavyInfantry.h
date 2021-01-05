#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CDynamic_Mesh;
class CShader;
class CPicking;
class COnHeadFont;
class CUD_HeavyInfantry :
	public CGameObject
{
public:
	enum STATE { STATE_IDLE = 0, STATE_SPON1 = 11, STATE_SPON2 = 12, STATE_END };
private:
	CUD_HeavyInfantry(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUD_HeavyInfantry(const CUD_HeavyInfantry& rhs);
	virtual ~CUD_HeavyInfantry() = default;
public:
	virtual HRESULT			Ready_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void			Render_GameObject();
public:
	static	CUD_HeavyInfantry*		Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
private:
	void					Change_State(STATE eID);
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
private:
	STATE					m_eCurState = STATE_END;
	STATE					m_ePreState = STATE_END;
};

