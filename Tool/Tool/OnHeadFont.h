#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CMyFont;
class COnHeadFont :
	public CGameObject
{
private:
	COnHeadFont(LPDIRECT3DDEVICE9 pGraphic_Device);
	COnHeadFont(const COnHeadFont& rhs);
	virtual ~COnHeadFont() = default;
public:
	virtual HRESULT			Ready_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta, _bool bCheck);
	virtual void			Render_GameObject();
public:
	static	COnHeadFont*	Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
private:
	HRESULT					Ready_Component();
public:
	void					SetPos(_vec3 vPos);
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CMyFont*				m_pFontCom = nullptr;
};

