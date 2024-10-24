#include "framework.h"
#include "Observer.h"
#include "Management.h"


CObserver::CObserver()
	: CComponent()
{
}

CObserver::CObserver(const CObserver& rhs)
	: CComponent(rhs)
{
}

HRESULT CObserver::Ready_Observer()
{
	return S_OK;
}

void CObserver::Update_Observer(DATA_TYPE eType, void* pData)
{
	list<void*>*	lstTemp = CManagement::GetInstance()->Get_List(eType);

	if (nullptr == lstTemp)
		return;

	auto iter_find = find(lstTemp->begin(), lstTemp->end(), pData);
	if (lstTemp->end() == iter_find)
		return;

	switch (eType)
	{
	case DATA_TYPE::DATA_INFO:
		m_tInfo = *reinterpret_cast<INFO*>(*iter_find);
		break;
	case DATA_TYPE::DATA_MATRIX:
		break;
	case DATA_TYPE::DATA_VECTOR:
		m_vec3 = *reinterpret_cast<_vec3*>(*iter_find);
		break;
	case DATA_TYPE::DATA_UI_INFO:
		m_Point = *reinterpret_cast<POINT*>(*iter_find);
		break;
	}

}

CObserver* CObserver::Create()
{
	CObserver* pInstance = new CObserver();
	if (FAILED(pInstance->Ready_Component()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* CObserver::Clone_Component(void* pArg)
{
	return new CObserver(*this);
}

void CObserver::Free()
{
	CComponent::Free();
}
