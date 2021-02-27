#pragma once
#include "Base.h"
class CGameObject;
class CComponent;

class CLayer :
    public CBase
{
private:
    CLayer();
    virtual ~CLayer() = default;
public:
	CComponent*					Get_ComponentPointer(const _tchar* pComponentTag, const _uint& iIndex);
public:
	HRESULT						Add_Object(CGameObject* pGameObject);
	HRESULT						Ready_Layer();
	_int						Update_Object(const _float& fTimeDelta);
	_int						LastUpdate_Object(const _float& fTimeDelta);
public:
	CGameObject*				Get_BackObject();
public:
	static CLayer*				Create();
	virtual void				Free();
public:
	list<CGameObject*>			Get_GameObjectLst() { return m_ObjectList; }
	CGameObject*				Get_GameObject(const _uint& iIdx);
private:
	list<CGameObject*>			m_ObjectList;
	typedef list<CGameObject*>	OBJECTLIST;
};

