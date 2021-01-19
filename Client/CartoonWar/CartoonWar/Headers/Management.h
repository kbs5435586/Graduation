#pragma once
#include "Component_Manager.h"
#include "GameObject_Manager.h"
#include "Light_Manager.h"
#include "FBXLoader.h"

#include "Base.h"
class CComponent;
class CGameObject;
class CLight;
class CScene;
class CManagement :
    public CBase
{
    _DECLARE_SINGLETON(CManagement);
private:
    CManagement();
    virtual ~CManagement() = default;
public:
	HRESULT					Ready_Management(const _uint& iNumScene);
	HRESULT					SetUp_ScenePointer(CScene* pNewScenePointer);
	_int					Update_Management(const _float& fTimeDelta);
	void					Render_Management();
public://ComponentManager
	CComponent*				Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex = 0);
	HRESULT					Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*				Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg = nullptr);
public://GameObjectManager
	HRESULT					Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag, CGameObject** ppCloneObject = nullptr, void* pArg = nullptr);
	HRESULT					Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
public://LightManager
	LIGHT*					Get_Light(const _tchar* pLightTag);
	HRESULT                 Add_LightInfo(const _tchar* pLightTag, LIGHT& tLightInfo);
public:	
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
	CLight_Manager*			m_pLight_Manager = nullptr;
};

