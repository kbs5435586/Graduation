#include "framework.h"
#include "Scene.h"
#include "GameObject_Manager.h"

CScene::CScene(ID3D12Device* pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pObject_Manager(CGameObject_Manager::GetInstance())
{
	m_pGraphic_Device->AddRef();
	m_pObject_Manager->AddRef();

}

HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	return m_pObject_Manager->Update_GameObject_Manager(fTimeDelta);
}

_int CScene::LastUpdate_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	return m_pObject_Manager->LastUpdate_GameObject_Manager(fTimeDelta);
}

void CScene::Render_Scene()
{
}

void CScene::Free()
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pGraphic_Device);
}
