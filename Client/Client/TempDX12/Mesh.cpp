#include "framework.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CComponent(rhs),
	m_pFbxScene(rhs.m_pFbxScene)

{
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

	if (m_pFbxScene)
		m_pFbxScene->Destroy();
	CComponent::Free();
}

