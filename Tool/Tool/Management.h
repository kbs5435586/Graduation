#pragma once
#include "Base.h"
#include "GameObject_Mgr.h"
#include "Component_Mgr.h"
#include "Light_Mgr.h"
class CScene;
class CComponent;
class CGameObject;
class CManagement :
	public CBase
{
	_DECLARE_SINGLETON(CManagement)
private:
	CManagement();
	virtual ~CManagement() = default;
public:
	HRESULT							Ready_Management(const _uint& iNumScene);
	HRESULT							Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
	HRESULT							Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag,void* pArg=nullptr, CGameObject** ppCloneObject = nullptr);
public:
	HRESULT							Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT							Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent, bool FLSE);
	CComponent*						Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg=nullptr);
	HRESULT							SetUp_ScenePointer(CScene* pNewScenePointer);
	HRESULT							Delete_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
public:
	void							Render_Management();
	_int							Update_Management(const _float& fTimeDelta);
	HRESULT							Clear_Layers(const _uint& iSceneID);
	HRESULT							Find_Clear_Layer(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	CGameObject*					Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIndex);
	list<CGameObject*>*				Get_ObjectList(const _uint& iSceneID, const _tchar* pLayerTag);
	CGameObject*					Get_BackObject(const _uint& iSceneID, const _tchar* pLayerTag);
public:
	map<const _tchar*, CLayer*>*	Get_MapLayer(const _uint& iSceneID);
public:					//KeyMgr
	void							Key_Update();
	bool							KeyUp(DWORD dwKey);
	bool							KeyDown(DWORD dwKey);
	bool							KeyPressing(DWORD dwKey);
	bool							KeyCombine(DWORD dwFirstKey, DWORD dwSecondKey);
public:
	CComponent*						Get_ComponentPointer(const _uint & iSceneID, const _tchar * pLayerTag, const _tchar * pComponentTag, const _uint & iIndex=0);
public:
	static void						Release_Engine();
public:
	virtual void					Free();
private:
	CScene*				m_pScene = nullptr;
	CGameObject_Mgr*	m_pGameObject_Mgr = nullptr;
	CComponent_Mgr*		m_pComponent_Mgr = nullptr;
private:
	CComponent*			m_pTempCom = nullptr;

private:
	DWORD m_dwKey = 0;
	DWORD m_dwKeyPressed = 0;
	DWORD m_dwKeyDown = 0;
};

