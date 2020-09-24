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
	HRESULT						Reserve_Object_Manager(const _uint& iNumScene);
	HRESULT						Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
	HRESULT						Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, 
		const _tchar* pLayerTag, CGameObject** ppCloneObject = nullptr, void* pArg = nullptr);
	_int						Update_GameObject_Manager(const _float& fTimeDelta);
	_int						LastUpdate_GameObject_Manager(const _float& fTimeDelta);
	HRESULT						Clear_Layers(const _uint& iSceneID);
private:
	CGameObject*				Find_Prototype(const _tchar* pGameObjectTag);
	CLayer*						Find_Layer(const _uint& iSceneID, const _tchar* pLayerTag);
	virtual void				Free();
private: // 원형객체들을 모아놓는다.
	map<const _tchar*, CGameObject*>			m_mapPrototype;
	typedef map<const _tchar*, CGameObject*>	MAPPROTOTYPE;
private: // 원형객체들을 복제한 객체를 레이어에 보관한다.
	map<const _tchar*, CLayer*>*				m_pMapLayers = nullptr;
	typedef map<const _tchar*, CLayer*>			MAPLAYERS;
private:
	_uint										m_iNumScene = 0;

};

