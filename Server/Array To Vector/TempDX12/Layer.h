#pragma once
#include "Base.h"
class CGameObject;
class CLayer :
    public CBase
{
private:
    CLayer();
    virtual ~CLayer() = default;
public:
	HRESULT						Add_Object(CGameObject* pGameObject);
	HRESULT						Ready_Layer();
	_int						Update_Object(const _float& fTimeDelta);
	_int						LastUpdate_Object(const _float& fTimeDelta);
public:
	static CLayer*				Create();
	virtual void				Free();
private:
	list<CGameObject*>			m_ObjectList;
	typedef list<CGameObject*>	OBJECTLIST;
};

