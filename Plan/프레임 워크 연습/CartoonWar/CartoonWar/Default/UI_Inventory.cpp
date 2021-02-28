#include "framework.h"
#include "UI_Inventory.h"
#include "Management.h"


CUI_Inventory::CUI_Inventory()
{
}

CUI_Inventory::CUI_Inventory(const CUI_Inventory& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Inventory::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_Inventory::Ready_GameObject(void* pArg)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	m_fX = 400.f;
	m_fY = 300.f;

	m_fSizeX = 300.f;
	m_fSizeY = 150.f;
	_vec3 vPos = _vec3(m_fX, m_fY, 0.f);
	float cellX{ m_fX }, cellY{ m_fY };
	for (int i = 0; i < 3; ++i)
	{
		vPos = _vec3(cellX, cellY, 0.f);
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_InventorySpace", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI", nullptr, &vPos)))
			return E_FAIL;
		cellX += 60;

	}
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Loading", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI",nullptr, &vPos)))
	//	return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Diffuse", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI")))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}

_int CUI_Inventory::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CUI_Inventory::LastUpdate_GameObject(const _float& fTimeDelta)
{

	return _int();
}

void CUI_Inventory::Render_GameObject()
{
	
}

HRESULT CUI_Inventory::CreateInputLayout()
{
	

	return S_OK;
}

CUI_Inventory* CUI_Inventory::Create()
{
	CUI_Inventory* pInstance = new CUI_Inventory();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CUI_Inventory Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Inventory::Clone_GameObject(void* pArg)
{
	CUI_Inventory* pInstance = new CUI_Inventory();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Inventory::Free()
{
}

HRESULT CUI_Inventory::Ready_Component()
{
	
	return S_OK;
}
