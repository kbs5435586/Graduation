#include "stdafx.h"
#include "Management.h"
#include "Scene.h"
#include "System.h"
#include "Timer_Mgr.h"
#include "Frame_Mgr.h"
#include "Device.h"
#include "GameObject_Mgr.h"
#include "Component_Mgr.h"
#include "Light_Mgr.h"
#include "input.h"
_IMPLEMENT_SINGLETON(CManagement)
CManagement::CManagement()
	: m_pComponent_Mgr(CComponent_Mgr::GetInstance())
	, m_pGameObject_Mgr(CGameObject_Mgr::GetInstance())
{
	m_pGameObject_Mgr->AddRef();
	m_pComponent_Mgr->AddRef();
}

HRESULT CManagement::Ready_Management(const _uint & iNumScene)
{
	if(nullptr == m_pGameObject_Mgr ||
		nullptr == m_pComponent_Mgr)
		return E_FAIL;

	if (FAILED(m_pGameObject_Mgr->Reserve_Object_Manager(iNumScene)))
		return E_FAIL;

	if (FAILED(m_pComponent_Mgr->Reserve_Component_Manager(iNumScene)))
		return E_FAIL;


	return S_OK;
}

HRESULT CManagement::Add_Prototype_GameObject(const _tchar * pGameObjectTag, CGameObject * pGameObject)
{
	if (nullptr == m_pGameObject_Mgr)
		return E_FAIL;

	return m_pGameObject_Mgr->Add_Prototype_GameObject(pGameObjectTag, pGameObject);
}

HRESULT CManagement::Add_GameObjectToLayer(const _tchar * pProtoTag, const _uint & iSceneID, const _tchar * pLayerTag, void* pArg, CGameObject ** ppCloneObject)
{
	if (nullptr == m_pGameObject_Mgr)
		return E_FAIL;

	return m_pGameObject_Mgr->Add_GameObjectToLayer(pProtoTag, iSceneID, pLayerTag, pArg, ppCloneObject);
}


HRESULT CManagement::Add_Prototype_Component(const _uint & iSceneID, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == m_pComponent_Mgr)
		return E_FAIL;

	return m_pComponent_Mgr->Add_Prototype_Component(iSceneID, pComponentTag, pComponent);
}

HRESULT CManagement::Add_Prototype_Component(const _uint & iSceneID, const _tchar * pComponentTag, CComponent * pComponent, bool FLSE)
{
	if (nullptr == m_pComponent_Mgr)
		return E_FAIL;
	m_pTempCom = pComponent;
	return m_pComponent_Mgr->Add_Prototype_Component(iSceneID, pComponentTag, pComponent);
}

CComponent * CManagement::Clone_Component(const _uint & iSceneID, const _tchar * pComponentTag, void* pArg)
{
	if (nullptr == m_pComponent_Mgr)
		return nullptr;

	return m_pComponent_Mgr->Clone_Component(iSceneID, pComponentTag, pArg);
}

HRESULT CManagement::SetUp_ScenePointer(CScene * pNewScenePointer)
{
	if (nullptr == pNewScenePointer)
		return E_FAIL;

	if (0 != Safe_Release(m_pScene))
		return E_FAIL;

	m_pScene = pNewScenePointer;

	m_pScene->AddRef();

	return NOERROR;

}

HRESULT CManagement::Delete_Prototype_Component(const _uint & iSceneID, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == m_pComponent_Mgr)
		return E_FAIL;

	return m_pComponent_Mgr->Delete_Prototype_Component(iSceneID, pComponentTag, pComponent);
}

_int CManagement::Update_Management(const _float & fTimeDelta)
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

HRESULT CManagement::Clear_Layers(const _uint & iSceneID)
{
	if (nullptr == m_pGameObject_Mgr)
		return E_FAIL;

	return m_pGameObject_Mgr->Clear_Layers(iSceneID);
}

HRESULT CManagement::Find_Clear_Layer(const _uint & iSceneID, const _tchar * pLayerTag)
{
	if (nullptr == m_pGameObject_Mgr)
		return E_FAIL;
	return m_pGameObject_Mgr->Find_Clear_Layer(iSceneID, pLayerTag);
}

CGameObject * CManagement::Get_GameObject(const _uint & iSceneID, const _tchar * pLayerTag, const _uint & iIndex)
{
	if (nullptr == m_pGameObject_Mgr)
		return nullptr;
	return m_pGameObject_Mgr->Get_GameObject(iSceneID, pLayerTag, iIndex);
}

list<CGameObject*>* CManagement::Get_ObjectList(const _uint & iSceneID, const _tchar * pLayerTag)
{
	if (nullptr == m_pGameObject_Mgr)
	{
#if _DEBUG
		_MSG_BOX("Can't Find ObjectList");
#endif 
		return nullptr;
	}

	return m_pGameObject_Mgr->Get_ObjectList(iSceneID, pLayerTag);
}

CGameObject * CManagement::Get_BackObject(const _uint & iSceneID, const _tchar * pLayerTag)
{
	return CGameObject_Mgr::GetInstance()->Get_BackObject(iSceneID, pLayerTag);
}

map<const _tchar*, CLayer*>* CManagement::Get_MapLayer(const _uint & iSceneID)
{
	if (m_pGameObject_Mgr != nullptr)
		return m_pGameObject_Mgr->Get_MapLayer(iSceneID);

	return &map<const _tchar*, CLayer*>();
}

void CManagement::Key_Update()
{
	m_dwKey = 0;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		m_dwKey |= KEY_LBUTTON;
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		m_dwKey |= KEY_RBUTTON;
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		m_dwKey |= KEY_RETURN;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		m_dwKey |= KEY_SHIFT;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		m_dwKey |= KEY_SPACE;

	if (GetAsyncKeyState('W') & 0x8000)
		m_dwKey |= KEY_UP;
	if (GetAsyncKeyState('S') & 0x8000)
		m_dwKey |= KEY_DOWN;
	if (GetAsyncKeyState('A') & 0x8000)
		m_dwKey |= KEY_LEFT;
	if (GetAsyncKeyState('D') & 0x8000)
		m_dwKey |= KEY_RIGHT;
	

	if (GetAsyncKeyState(VK_F1) & 0x8000)
		m_dwKey |= KEY_F1;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
		m_dwKey |= KEY_F2;
	if (GetAsyncKeyState(VK_F3) & 0x8000)
		m_dwKey |= KEY_F3;
	if (GetAsyncKeyState('U') & 0x8000)
		m_dwKey |= KEY_U;
}

bool CManagement::KeyUp(DWORD dwKey)
{
	if (m_dwKey & dwKey)
	{
		m_dwKeyPressed |= dwKey;
		return false;
	}
	else if (m_dwKeyPressed & dwKey)
	{
		m_dwKeyPressed ^= dwKey;
		return true;
	}

	return false;
}

bool CManagement::KeyDown(DWORD dwKey)
{
	if (!(m_dwKeyDown & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyDown |= dwKey;
		return true;
	}
	else if (!(m_dwKey & dwKey) && (m_dwKeyDown & dwKey))
	{
		m_dwKeyDown ^= dwKey;
		return false;
	}

	return false;
}

bool CManagement::KeyPressing(DWORD dwKey)
{
	if (m_dwKey & dwKey)
		return true;

	return false;
}

bool CManagement::KeyCombine(DWORD dwFirstKey, DWORD dwSecondKey)
{
	if (KeyDown(dwSecondKey) && (m_dwKey & dwFirstKey))
		return true;

	return false;
}

CComponent * CManagement::Get_ComponentPointer(const _uint & iSceneID, const _tchar * pLayerTag, const _tchar * pComponentTag, const _uint & iIndex)
{
	if (nullptr == m_pGameObject_Mgr)
		return nullptr;

	return m_pGameObject_Mgr->Get_ComponentPointer(iSceneID, pLayerTag, pComponentTag, iIndex);
}

void CManagement::Release_Engine()
{
	_ulong			dwRefCnt = 0;

	if (dwRefCnt = CManagement::GetInstance()->DestroyInstance())
		_MSG_BOX("CManagement Release Failed");
	if (dwRefCnt = CLight_Mgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CLight_Mgr Release Failed");
	if (dwRefCnt = CGameObject_Mgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CObject_Manager Release Failed");
	if (dwRefCnt = CComponent_Mgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CComponent_Manager Release Failed");
	if (dwRefCnt = CSystem::GetInstance()->DestroyInstance())
		_MSG_BOX("CSystem Release Failed");
	if (dwRefCnt = CTimer_Mgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CTimer_Manager Release Failed");
	if (dwRefCnt = CFrame_Mgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CTimer_Manager Release Failed");
	if (dwRefCnt = CInput::GetInstance()->DestroyInstance())
		_MSG_BOX("CInput Release Failed");
	if (dwRefCnt = CDevice::GetInstance()->DestroyInstance())
		_MSG_BOX("CGraphic_Device Release Failed");
}

void CManagement::Free()
{
	Safe_Release(m_pComponent_Mgr);
	Safe_Release(m_pGameObject_Mgr);
	Safe_Release(m_pScene);
}
