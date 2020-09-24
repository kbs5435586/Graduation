#include "framework.h"
#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_Object(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);


	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Object(const _float& fTimeDelta)
{
	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->Update_GameObject(fTimeDelta) & 0x80000000)
				return -1;
		}
	}
	return _int();
}

_int CLayer::LastUpdate_Object(const _float& fTimeDelta)
{
	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->LastUpdate_GameObject(fTimeDelta) & 0x80000000)
				return -1;
		}
	}

	return _int();
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		MessageBox(0, L"CLayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
	{
		Safe_Release(pGameObject);
	}
	m_ObjectList.clear();
}
