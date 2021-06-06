#pragma once
#include "Component_Manager.h"
#include "GameObject_Manager.h"
#include "Light_Manager.h"
#include "Constant_Buffer_Manager.h"
#include "ObserverManager.h"
#include "RTTMananger.h"
#include "KeyManager.h"
#include "LoadManager.h"
#include "FBXLoader.h"
#include "UAVManager.h"
#include "CollisionMgr.h"

#include "MRT.h"
#include "Base.h"
class CComponent;
class CGameObject;
class CLight;
class CScene;
class CRenderer;
class CManagement :
    public CBase
{
    _DECLARE_SINGLETON(CManagement);
private:
    CManagement();
    virtual ~CManagement() = default;
public:
	HRESULT						Ready_Management(const _uint& iNumScene);
	HRESULT						SetUp_ScenePointer(CScene* pNewScenePointer);
	_int						Update_Management(const _float& fTimeDelta);
	void						Render_Management();

public://ComponentManager
	CComponent*					Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex = 0);
	HRESULT						Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*					Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg = nullptr);
public://GameObjectManager
	HRESULT						Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag, CGameObject** ppCloneObject = nullptr, void* pArg = nullptr, _uint iIdx=0);
	HRESULT						Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject);
	CGameObject*				Get_BackObject(const _uint& iSceneID, const _tchar* pLayerTag);
	CLayer*						Get_Layer(const _uint& iSceneID, const _tchar* pLayerTag);
public://LightManager
	LIGHT						Get_Light(_uint& iIdx);
	HRESULT						Add_LightInfo(LIGHT& tLightInfo);
	void						SetUp_OnShader_Light();
	void						Update();
	void						Render();
public://ConstantBuffer_Mananger
	HRESULT						Create_Constant_Buffer(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType, _bool IsGlobal = false);
	CConstant_Buffer*			GetConstantBuffer(_uint iIdx) { return m_pConstant_Buffer_Manager->GetConstantBuffer(iIdx); }
	vector<CConstant_Buffer*>	GetConstantBuffer() { return m_pConstant_Buffer_Manager->GetConstantBuffer(); }
public://RTT_Manager
	HRESULT						Ready_RTT_Manager();
	void                        Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv);
	CMRT*						Get_RTT(const _uint& iIdx);
public://Key_Manager
	void						Key_Update();
	_bool						Key_Down(DWORD dwKey);
	_bool						Key_Up(DWORD dwKey);
	_bool						Key_Pressing(DWORD dwKey);
	_bool						Key_Combine(DWORD dwFirstKey, DWORD dwSecondKey);
public://Observer_Manager
	list<void*>*				Get_List(DATA_TYPE eType);
	void						Add_Data(DATA_TYPE eType, void* pData);
	void                        Remove_Data(DATA_TYPE eType, void* pData);
	void                        Subscribe(CObserver* pObserver);
	void                        UnSubscribe(CObserver* pObserver);
	void                        Notify(DATA_TYPE eType, void* pData = nullptr);
public://Load_Manager
	HRESULT						Load_File(const _tchar* pFilePath, void* pArg = nullptr);
	HRESULT						Load_File_Low(const _tchar* pFilePath, void* pArg = nullptr);
	HRESULT						Load_File_Hatch(const _tchar* pFilePath, void* pArg = nullptr);
public://UAV_Manager
	HRESULT						Ready_UAV_Manager();
	CUAV*						Get_UAV(const _tchar* pUAVTag);
public://CCollisionMgr
	HRESULT						Ready_CollsionManager();
	void						Update_CollisionManager();
public:
	CScene*						Get_Scene(){return m_pScene;}
public:	
	HRESULT						Clear_Layers(const _uint& iSceneID);
	static void					Release_Engine();
public:
	CGameObject*				Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIdx);
	list<CGameObject*>			Get_GameObjectLst(const _uint& iSceneID, const _tchar* pLayerTag);
private:
	virtual void				Free();
private:
	CScene*						m_pScene = nullptr;
	CGameObject_Manager*		m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
	CConstant_Buffer_Manager*	m_pConstant_Buffer_Manager = nullptr;
	CRTTMananger*				m_pRTT_Mananger = nullptr;
	CKeyManager*				m_pKey_Manager = nullptr;
	CObserverManager*			m_pObserver_Manager = nullptr;
	CLoadManager*				m_pLoad_Manager = nullptr;
	CUAVManager*				m_pUAV_Manager = nullptr;
	CCollisionMgr*				m_pCollision_Manager = nullptr;
	//CServer_Manager*			m_pServer_Manager = nullptr;
};

