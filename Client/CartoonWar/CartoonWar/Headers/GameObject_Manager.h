#pragma once
#include "Base.h"
class CLayer;
class CGameObject;
class CComponent;
class CGameObject_Manager :
    public CBase
{
	_DECLARE_SINGLETON(CGameObject_Manager)
private:
	CGameObject_Manager();
	virtual ~CGameObject_Manager() = default;
public:
	CComponent*									Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex);
public:
	HRESULT										Reserve_Object_Manager(const _uint& iNumScene);
	HRESULT										Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
	HRESULT										Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, 
																	const _tchar* pLayerTag, CGameObject** ppCloneObject = nullptr, void* pArg = nullptr, _uint iIdx = 0);
	_int										Update_GameObject_Manager(const _float& fTimeDelta);
	_int										LastUpdate_GameObject_Manager(const _float& fTimeDelta);
	HRESULT										Clear_Layers(const _uint& iSceneID);
public:
	CLayer*										Find_Layer(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	CGameObject*								Get_BackObject(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	CGameObject*								Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIdx);
	list<CGameObject*>							Get_GameObjectLst(const _uint& iSceneID, const _tchar* pLayerTag);
private:
	CGameObject*								Find_Prototype(const _tchar* pGameObjectTag);
	
	virtual void								Free();
private:
	map<const _tchar*, CGameObject*>			m_mapPrototype;
	typedef map<const _tchar*, CGameObject*>	MAPPROTOTYPE;
private:
	map<const _tchar*, CLayer*>*				m_pMapLayers = nullptr;
	typedef map<const _tchar*, CLayer*>			MAPLAYERS;
private:
	_uint										m_iNumScene = 0;

};

