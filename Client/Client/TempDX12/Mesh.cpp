#include "framework.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CMesh::CMesh(const CMesh& rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
{

	m_IsClone = true;
}

HRESULT CMesh::Ready_VIBuffer()
{
	return S_OK;
}

CComponent* CMesh::Clone_Component(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	if (m_IsClone)
		if (m_pScene)
		{
			m_pScene->Destroy();
			m_pScene = nullptr;
		}

	CComponent::Free();
}

