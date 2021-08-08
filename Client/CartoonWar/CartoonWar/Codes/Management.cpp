#include "framework.h"
#include "Management.h"
#include "Scene.h"
#include "System.h"
#include "TimerManager.h"
#include "FrameManager.h"
#include "Constant_Buffer_Manager.h"
#include "SoundMgr.h"


_IMPLEMENT_SINGLETON(CManagement)
CManagement::CManagement()
	: m_pObject_Manager(CGameObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pConstant_Buffer_Manager(CConstant_Buffer_Manager::GetInstance())
	, m_pRTT_Mananger(CRTTMananger::GetInstance())
	, m_pKey_Manager(CKeyManager::GetInstance())
	, m_pObserver_Manager(CObserverManager::GetInstance())
	, m_pLoad_Manager(CLoadManager::GetInstance())
	, m_pUAV_Manager(CUAVManager::GetInstance())
	, m_pCollision_Manager(CCollisionMgr::GetInstance())
	, m_pFont_Manager(CFontMgr::GetInstance())
	, m_pSound_Manager(CSoundMgr::GetInstance())
	, m_pEvent_Manager(CEventMgr::GetInstance())
	//, m_pServer_Manager(CServer_Manager::GetInstance())
{
	m_pObject_Manager->AddRef();
	m_pComponent_Manager->AddRef();
	m_pLight_Manager->AddRef();
	m_pConstant_Buffer_Manager->AddRef();
	m_pRTT_Mananger->AddRef();
	m_pKey_Manager->AddRef();
	m_pObserver_Manager->AddRef();
	m_pLoad_Manager->AddRef();
	m_pUAV_Manager->AddRef();
	m_pCollision_Manager->AddRef();
	m_pFont_Manager->AddRef();
	m_pSound_Manager->AddRef();
	m_pEvent_Manager->AddRef();
	//m_pServer_Manager->AddRef();
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

CGameObject* CManagement::Get_BackObject(const _uint& iSceneID, const _tchar* pLayerTag)
{
	return m_pObject_Manager->Get_BackObject(iSceneID, pLayerTag);
}

CLayer* CManagement::Get_Layer(const _uint& iSceneID, const _tchar* pLayerTag)
{
	return m_pObject_Manager->Find_Layer(iSceneID, pLayerTag);
}

LIGHT CManagement::Get_Light(_uint& iIdx)
{
	return m_pLight_Manager->GetLight(iIdx);
}

HRESULT CManagement::Add_LightInfo(LIGHT& tLightInfo)
{
	return m_pLight_Manager->Add_LightInfo(tLightInfo);
}



void CManagement::SetUp_OnShader_Light()
{
	return m_pLight_Manager->SetUp_OnShader();
}

void CManagement::Update()
{
	return m_pLight_Manager->Update();
}

void CManagement::Render()
{
	return m_pLight_Manager->Render();
}



HRESULT CManagement::Create_Constant_Buffer(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType, _bool IsGlobal)
{
	return m_pConstant_Buffer_Manager->Create_Constant_Buffer(iBufferSize, iMaxCnt, eType, IsGlobal);
}

HRESULT CManagement::Ready_RTT_Manager()
{
	return m_pRTT_Mananger->Ready_RTTMananger();
}

void CManagement::Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv)
{
	m_pRTT_Mananger->Set_RenderTarget(pRTT_Tag, pDsv);
}

CMRT* CManagement::Get_RTT(const _uint& iIdx)
{
	return m_pRTT_Mananger->Get_RTT(iIdx);
}


void CManagement::Key_Update()
{
	m_pKey_Manager->Key_Update();
}

_bool CManagement::Key_Down(DWORD dwKey)
{
	return m_pKey_Manager->Key_Down(dwKey);
}

_bool CManagement::Key_Up(DWORD dwKey)
{
	return m_pKey_Manager->Key_Up(dwKey);
}

_bool CManagement::Key_Pressing(DWORD dwKey)
{
	return m_pKey_Manager->Key_Pressing(dwKey);
}

_bool CManagement::Key_Combine(DWORD dwFirstKey, DWORD dwSecondKey)
{
	return m_pKey_Manager->Key_Combine(dwFirstKey, dwSecondKey);
}

list<void*>* CManagement::Get_List(DATA_TYPE eType)
{
	return m_pObserver_Manager->Get_List(eType);
}

void CManagement::Add_Data(DATA_TYPE eType, void* pData)
{
	return m_pObserver_Manager->Add_Data(eType, pData);
}

void CManagement::Remove_Data(DATA_TYPE eType, void* pData)
{
	return m_pObserver_Manager->Remove_Data(eType, pData);
}

void CManagement::Subscribe(CObserver* pObserver)
{
	return m_pObserver_Manager->Subscribe(pObserver);
}

void CManagement::UnSubscribe(CObserver* pObserver)
{
	return m_pObserver_Manager->UnSubscribe(pObserver);
}

void CManagement::Notify(DATA_TYPE eType, void* pData)
{
	return m_pObserver_Manager->Notify(eType, pData);
}

HRESULT CManagement::Load_File(const _tchar* pFilePath, void* pArg)
{
	return m_pLoad_Manager->Load_File(pFilePath, pArg);
}

HRESULT CManagement::Load_File_Low(const _tchar* pFilePath, void* pArg)
{
	return m_pLoad_Manager->Load_File_Low(pFilePath, pArg);
}

HRESULT CManagement::Load_File_Hatch(const _tchar* pFilePath, void* pArg)
{
	return m_pLoad_Manager->Load_File_Hatch(pFilePath, pArg);
}

HRESULT CManagement::Load_File_Castle(const _tchar* pFilePath, void* pArg)
{
	return m_pLoad_Manager->Load_File_Castle(pFilePath, pArg);;
}

HRESULT CManagement::Ready_UAV_Manager()
{
	return m_pUAV_Manager->Ready_UAVManager();
}

CUAV* CManagement::Get_UAV(const _tchar* pUAVTag)
{
	return m_pUAV_Manager->GetUAV(pUAVTag);
}

HRESULT CManagement::Ready_CollsionManager()
{
	return m_pCollision_Manager->Ready_CollsionManager();
}

void CManagement::Update_CollisionManager()
{
	return m_pCollision_Manager->Update_CollisionManager();
}

HRESULT CManagement::Ready_FontMgr(const char* pFilePath)
{
	return m_pFont_Manager->Ready_FontMgr(pFilePath);
}

HRESULT CManagement::Create_Font_Buffer(const _tchar* pFontTag, const char* pSentence, float iDrawX, float iDrawY)
{
	return m_pFont_Manager->Create_Buffer(pFontTag, pSentence, iDrawX, iDrawY);
}

void CManagement::Render_Font()
{
	return m_pFont_Manager->Render_Font();
}

void CManagement::Delete_Font(const _tchar* pFontTag)
{
	return m_pFont_Manager->Delete_Font(pFontTag);
}

void CManagement::Delete_All_Font()
{
	return m_pFont_Manager->Delete_All_Fo1nt();
}

HRESULT CManagement::Ready_Channel()
{
	return m_pSound_Manager->Ready_Channel();
}

HRESULT CManagement::Add_Sound(Sound_Character eCharacter, SoundState State, const char* pFilePath, const _float fVolume)
{
	return m_pSound_Manager->Add_Sound(eCharacter, State, pFilePath, fVolume);
}

HRESULT CManagement::Add_BGSound(Sound_Character eCharacter, SoundState State, const char* pFilePath, const _float fVolume)
{
	return m_pSound_Manager->Add_BGSound(eCharacter, State, pFilePath, fVolume);
}

void CManagement::Play_Sound(Sound_Character eCharacter, SoundState State, FMOD_MODE eMode )
{
	return m_pSound_Manager->Play_Sound(eCharacter, State, eMode);
}

void CManagement::Play_BGSound(Sound_Character eCharacter, SoundState State)
{
	return m_pSound_Manager->Play_BGSound(eCharacter, State);
}

void CManagement::Pause_Sound()
{
	return m_pSound_Manager->Pause_Sound();
}

void CManagement::Pause_BGSound()
{
	return m_pSound_Manager->Pause_BGSound();
}

void CManagement::Play_Sound(SoundChannel eChannel, Sound_Character eCharacter, SoundState State, const _float& fVolume, FMOD_MODE eMode)
{
	return m_pSound_Manager->Play_Sound(eChannel, eCharacter, State, fVolume, eMode);
}

void CManagement::SetVolume(SoundChannel eChannel, const _float& fVolume)
{
	return m_pSound_Manager->SetVolume(eChannel, fVolume);
}

void CManagement::Pause_Sound(SoundChannel eChannel)
{
	return m_pSound_Manager->Pause_Sound(eChannel);
}

void CManagement::Stop_Sound(SoundChannel eChannel)
{
	return m_pSound_Manager->Stop_Sound(eChannel);
}

void CManagement::Update_Sound()
{
	return m_pSound_Manager->Update_Sound();
}

void CManagement::Update_Event_Manager()
{
	return m_pEvent_Manager->Update_EventMgr();
}




HRESULT CManagement::Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype_Component(iSceneID, pComponentTag, pComponent);
}

HRESULT CManagement::Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag,
	CGameObject** ppCloneObject, void* pArg , _uint iIdx)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pProtoTag, iSceneID, pLayerTag, ppCloneObject, pArg, iIdx);
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
	
	m_pKey_Manager->Key_Update();

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


	if (dwRefCnt = CSoundMgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CSoundMgr Release Failed");
	if (dwRefCnt = CEventMgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CEventMgr Release Failed");

	if (dwRefCnt = CFontMgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CFontMgr Release Failed");

	if (dwRefCnt = CCollisionMgr::GetInstance()->DestroyInstance())
		_MSG_BOX("CCollisionMgr Release Failed");

	if (dwRefCnt = CObserverManager::GetInstance()->DestroyInstance())
		_MSG_BOX("CObserverManager Release Failed");

	if (dwRefCnt = CUAVManager::GetInstance()->DestroyInstance())
		_MSG_BOX("CUAVManager Release Failed");

	if (dwRefCnt = CLight_Manager::GetInstance()->DestroyInstance())
		_MSG_BOX("CLight_Manager Release Failed");

	if (dwRefCnt = CLoadManager::GetInstance()->DestroyInstance())
		_MSG_BOX("CLoadManager Release Failed");

	if (dwRefCnt = CRTTMananger::GetInstance()->DestroyInstance())
		_MSG_BOX("CRTTMananger Release Failed");

	if (dwRefCnt = CConstant_Buffer_Manager::GetInstance()->DestroyInstance())
		_MSG_BOX("CConstant_Buffer_Manager Release Failed");

	if (dwRefCnt = CGameObject_Manager::GetInstance()->DestroyInstance())
		_MSG_BOX("CGameObject_Manager Release Failed");

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

	//if (dwRefCnt = CServer_Manager::GetInstance()->DestroyInstance())
	//	_MSG_BOX("CServer_Manager Release Failed");
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
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pUAV_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pConstant_Buffer_Manager);
	Safe_Release(m_pRTT_Mananger);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pObserver_Manager);
	Safe_Release(m_pLoad_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pScene);
	//Safe_Release(m_pServer_Manager);
}
