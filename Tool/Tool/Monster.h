#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RcTex;
class CTexture;
class CPicking;

class CMonster :
	public CGameObject
{
private:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& rhs);
	virtual ~CMonster()=default;
public:
	virtual HRESULT		Ready_Prototype();
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual _int		LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
public:
	static	CMonster*	Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject();
	virtual void		Free();
private:
	HRESULT				Ready_Component();
private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CBuffer_RcTex*		m_pBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CPicking*			m_pPickingCom = nullptr;
};

