#pragma once
#include "Base.h"
class CShader;
class CComponent;
class CGameObject :
    public CBase
{
protected:
    CGameObject();
    CGameObject(const CGameObject& rhs);
	virtual ~CGameObject()=default;
public:
	CComponent*								Get_ComponentPointer(const _tchar* pComponentTag);
public:
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
public:
	INFO&									GetInfo(){return m_tInfo;}
public:
	virtual HRESULT							CreateInputLayout();
protected:
	HRESULT									Add_Component(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*								Find_Component(const _tchar* pComponentTag);
public:
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr) = 0;
	virtual void							Free();
protected:
	map<const _tchar*, CComponent*>			m_mapComponent;
	typedef map<const _tchar*, CComponent*>	MAPCOMPONENT;
protected:
	INFO									m_tInfo = {};
};

