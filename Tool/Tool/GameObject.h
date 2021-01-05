#pragma once
#include "Base.h"

#include "MainFrm.h"
#include "SecondWnd.h"
#include "AnimationTool.h"

class CComponent;
class CGameObject :
	public CBase
{
protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject()=default;

public:
	virtual HRESULT		Ready_GameObject(void* pArg);
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual _int		LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
public:
	CComponent*			Get_ComponentPointer(const _tchar* pComponentTag);
	_uint				Get_AnimationNum() { return m_iAnimationNum; }
public:
	void				Set_AnimationNUm(_uint iAnimationNum) { m_iAnimationNum = iAnimationNum; }
protected:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
public:
	LPDIRECT3DDEVICE9	Get_Device() { return m_pGraphic_Device; }
protected:
	map<const _tchar*, CComponent*>			m_mapComponent;
protected:
	CComponent*			Find_Component(const _tchar* pComponentTag);
public:
	HRESULT				Add_Component(const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT				Delete_Component(const _tchar* pComponentTag, CComponent* pComponent);
protected:
	const _tchar*		m_pCurrent_TextureTag = L"";
public:
	const _tchar*		Get_Current_TextureTag() { return m_pCurrent_TextureTag; }
protected:		//UI
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
protected:		//Camera
	_float				m_fCameraSpeed = 10.f;
protected:
	_vec3				m_vecPickingPos = {};
	_bool				m_IsPick = false;
	_uint				m_iAnimationNum = 0;
	_uint				m_iNewAnimationNum = 0;
protected:
	_bool				m_IsUI = false;
	_float				m_fMove = 1.f;
protected:
	_bool				m_IsLoadUI_Texture = false;
protected:
	_tchar*				m_pGameObject_Tag = L"";
	_tchar*				m_pComponent_Tag = L"";
protected:
	UI					m_eUI = UI::UI_END;
protected:				//Animation
	_bool				m_IsOnce = false;
	_float				m_fOnIdleTime = 0.f;
public:
	_bool&				GetIsPick() { return m_IsPick; }
	_bool&				GetIsUI() { return m_IsUI; }
	_bool&				GetLoadUI_Texture() { return m_IsLoadUI_Texture; }
	_float&				Get_FX() { return m_fX; }
	_float&				Get_FY() { return m_fY; }
	_float&				Get_SizeX() { return m_fSizeX; }
	_float&				Get_SizeY() { return m_fSizeY; }
	_tchar*				Get_GameObject_Tag() { return m_pGameObject_Tag; }
	_tchar*&			Get_Component_Tag() { return m_pComponent_Tag; }
	UI&					GetEnumUI() { return m_eUI; }
	_float&				GetCameraSpeed() { return m_fCameraSpeed; }

public:
	void				Set_Component_Tag(_tchar* pComponent_Tag){m_pComponent_Tag = pComponent_Tag;}
public:
	virtual CGameObject* Clone_GameObject(void* pArg) = 0;
protected:
	virtual void Free();
};

