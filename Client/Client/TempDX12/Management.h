#pragma once
#include "Component_Manager.h"
#include "GameObject_Manager.h"
#include "Light_Manager.h"
#include "Server_Manager.h"

#include "Base.h"
class CComponent;
class CGameObject;
class CScene;
class CManagement :
    public CBase
{
    _DECLARE_SINGLETON(CManagement);
private:
    CManagement();
    virtual ~CManagement() = default;
public:
	CComponent*				Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex = 0);
public:
	HRESULT					Ready_Management(const _uint& iNumScene);
	HRESULT					Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
	HRESULT					Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT					Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag, CGameObject** ppCloneObject = nullptr, void* pArg = nullptr);
public:
	CComponent*				Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg = nullptr);
	HRESULT					SetUp_ScenePointer(CScene* pNewScenePointer);
	_int					Update_Management(const _float& fTimeDelta);
	void					Render_Management();
	HRESULT					Clear_Layers(const _uint& iSceneID);
	static void				Release_Engine();
public:
	CGameObject*			Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIdx);
	list<CGameObject*>		Get_GameObjectLst(const _uint& iSceneID, const _tchar* pLayerTag);
private:
	virtual void			Free();
private:
	CScene*					m_pScene = nullptr;
	CGameObject_Manager*	m_pObject_Manager = nullptr;
	CComponent_Manager*		m_pComponent_Manager = nullptr;
	//CServer_Manager*		m_pServer_Manager = nullptr;
};

