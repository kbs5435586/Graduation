#include "framework.h"
#include "Management.h"
#include "Scene.h"
#include "System.h"
#include "TimerManager.h"
#include "FrameManager.h"


_IMPLEMENT_SINGLETON(CManagement)
CManagement::CManagement()
	: m_pObject_Manager(CGameObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
{
	m_pObject_Manager->AddRef();
	m_pComponent_Manager->AddRef();
}

CComponent* CManagement::Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPointer(iSceneID, pLayerTag, pComponentTag, iIndex);
}

HRESULT CManagement::Ready_Management(const _uint& iNumScene)
{

	if (nullptr == m_pObject_Manager || nullptr == m_pComponent_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Object_Manager(iNumScene)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Component_Manager(iNumScene)))
		return E_FAIL;



	return NOERROR;
}

HRESULT CManagement::Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype_GameObject(pGameObjectTag, pGameObject);
}

HRESULT CManagement::Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype_Component(iSceneID, pComponentTag, pComponent);
}

HRESULT CManagement::Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag,
	CGameObject** ppCloneObject, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pProtoTag, iSceneID, pLayerTag, ppCloneObject, pArg);
}

CComponent* CManagement::Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iSceneID, pComponentTag, pArg);
}

HRESULT CManagement::SetUp_ScenePointer(CScene* pNewScenePointer)
{
	if (nullptr == pNewScenePointer)
		return E_FAIL;

	if (0 != Safe_Release(m_pScene))
		return E_FAIL;

	m_pScene = pNewScenePointer;

	m_pScene->AddRef();

	return NOERROR;
}

_int CManagement::Update_Management(const _float& fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	_int	iProcessCodes = 0;

	iProcessCodes = m_pScene->Update_Scene(fTimeDelta);
	if (iProcessCodes & 0x80000000)
		return iProcessCodes;

	iProcessCodes = m_pScene->LastUpdate_Scene(fTimeDelta);
	if (iProcessCodes & 0x80000000)
		return iProcessCodes;

	return _int(0);
}

void CManagement::Render_Management()
{
	if (nullptr == m_pScene)
		return;

	m_pScene->Render_Scene();
}

HRESULT CManagement::Clear_Layers(const _uint& iSceneID)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clear_Layers(iSceneID);
}

void CManagement::Release_Engine()
{
	_ulong			dwRefCnt = 0;

	if (dwRefCnt = CManagement::GetInstance()->DestroyInstance())
		_MSG_BOX("CManagement Release Failed");

	if (dwRefCnt = CGameObject_Manager::GetInstance()->DestroyInstance())
		_MSG_BOX("CObject_Manager Release Failed");

	if (dwRefCnt = CComponent_Manager::GetInstance()->DestroyInstance())
		_MSG_BOX("CComponent_Manager Release Failed");

	if (dwRefCnt = CSystem::GetInstance()->DestroyInstance())
		_MSG_BOX("CSystem Release Failed");

	if (dwRefCnt = CTimerManager::GetInstance()->DestroyInstance())
		_MSG_BOX("CTimer_Manager Release Failed");

	if (dwRefCnt = CFrameManager::GetInstance()->DestroyInstance())
		_MSG_BOX("CTimer_Manager Release Failed");

	if (dwRefCnt = CInput::GetInstance()->DestroyInstance())
		_MSG_BOX("CInput Release Failed");

	if (dwRefCnt = CCamera_Manager::GetInstance()->DestroyInstance())
		_MSG_BOX("CCamera_Manager Release Failed");

	if (dwRefCnt = CDevice::GetInstance()->DestroyInstance())
		_MSG_BOX("CDevice Release Failed");
}

CGameObject* CManagement::Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIdx)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(iSceneID, pLayerTag, iIdx);
}

list<CGameObject*> CManagement::Get_GameObjectLst(const _uint& iSceneID, const _tchar* pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return list<CGameObject*>();

	return m_pObject_Manager->Get_GameObjectLst(iSceneID, pLayerTag);
}

void CManagement::Free()
{
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pScene);
}
