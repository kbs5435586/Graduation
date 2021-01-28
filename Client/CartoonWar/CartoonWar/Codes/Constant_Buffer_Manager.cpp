#include "framework.h"
#include "Constant_Buffer_Manager.h"

_IMPLEMENT_SINGLETON(CConstant_Buffer_Manager)

CConstant_Buffer_Manager::CConstant_Buffer_Manager()
{

}

HRESULT CConstant_Buffer_Manager::Create_Constant_Buffer(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType)
{
	CConstant_Buffer* pInstance = CConstant_Buffer::Create(iBufferSize, iMaxCnt, eType);
	if (nullptr == pInstance)
	{
		return E_FAIL;
	}
	m_vecConstantBuffer.push_back(pInstance);

	return S_OK;
}

void CConstant_Buffer_Manager::Free()
{
	for (auto& iter : m_vecConstantBuffer)
		Safe_Release(iter);
}
