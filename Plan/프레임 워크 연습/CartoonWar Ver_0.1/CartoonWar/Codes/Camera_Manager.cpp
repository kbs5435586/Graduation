#include "framework.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "Management.h"
_IMPLEMENT_SINGLETON(CCamera_Manager)


CCamera_Manager::CCamera_Manager()
{
	m_matOrtho = XMMatrixOrthographicLH((_float)WINCX, (_float)WINCY, 0.f, 1.f);
}

_matrix CCamera_Manager::Get_ReflectMatrix(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIdx, const _float& fHeight)
{
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return _matrix();
	pManagement->AddRef();

	_matrix matTemp = dynamic_cast<CCamera*>(pManagement->Get_GameObject(iSceneID, pLayerTag, iIdx))->Calculate_RelfectMatrix(fHeight);

	Safe_Release(pManagement);
	return matTemp;
}



void CCamera_Manager::Free()
{
}
