#pragma once
#include "Base.h"
class CComponent :
	public CBase
{
protected:
	CComponent(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;
public:
	HRESULT Ready_Component();
	virtual CComponent* Clone_Component(void* pArg)PURE;
	virtual void Free();
public:
	_tchar*						Get_ObjectName() { return m_pGameObjectName; }
	_tchar*						Get_Object_FileName()  { return m_pObject_FileName; }
	_tchar*						Get_ComponentTag() { return m_pComponent_Tag; }
	_bool&						Get_IsDyanmic() { return m_IsDynamic; }
protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	_bool						m_isClone = false;
protected:
	_tchar*						m_pGameObjectName = L"";
	_tchar*						m_pObject_FileName = L"";
	_tchar*						m_pComponent_Tag = L"";
	_bool						m_IsDynamic = false;

};

