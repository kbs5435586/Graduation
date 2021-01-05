#pragma once
#include "Base.h"
class CLayer;
class CGameObject;
class CComponent;
class CGameObject_Mgr :
	public CBase
{
	_DECLARE_SINGLETON(CGameObject_Mgr)
public:
	CGameObject_Mgr();
	virtual ~CGameObject_Mgr()=default;
public:
	HRESULT							Reserve_Object_Manager(const _uint& iNumScene);
	HRESULT							Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
	HRESULT							Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag, void* pArg = nullptr, CGameObject** ppCloneObject = nullptr);
	_int							Update_GameObject_Manager(const _float& fTimeDelta);
	_int							LastUpdate_GameObject_Manager(const _float& fTimeDelta);
	HRESULT							Clear_Layers(const _uint& iSceneID);
private:
	CGameObject*					Find_Prototype(const _tchar* pGameObjectTag);
	CLayer*							Find_Layer(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	CComponent*						Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIdx);
	list<CGameObject*>*				Get_ObjectList(const _uint& iSceneID, const _tchar* pLayerTag);
	CGameObject*					Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIndex);
	CGameObject*					Get_BackObject(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	map<const _tchar*, CLayer*>*	Get_MapLayer(const _uint& iSceneID);
public:
	HRESULT							Find_Clear_Layer(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	virtual void					Free();
private:
	map<const _tchar*, CGameObject*>m_mapPrototype;
	map<const _tchar*, CLayer*>*	m_pMapLayers = nullptr;
	_uint							m_iNumScene = 0;
private:
	typedef map<const _tchar*, CGameObject*>	MAPPROTOTYPE;
	typedef map<const _tchar*, CLayer*>	MAPLAYERS;
};

