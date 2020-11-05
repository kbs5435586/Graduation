#include "framework.h"
#include "Dynamic_Mesh.h"
#include "Animation_Controller.h"
#include "Loader_Test.h"

CDynamic_Mesh::CDynamic_Mesh(ID3D12Device* pGraphic_Device)
	: CMesh(pGraphic_Device)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh& rhs)
	: CMesh(rhs)
	, m_pLoader(rhs.m_pLoader)
	, m_pScene(rhs.m_pScene)
{
	m_pLoader->AddRef();
	m_IsClone = true;
}

HRESULT CDynamic_Mesh::Ready_Dynamic_Mesh(string strFilePath)
{
	m_pLoader = CLoader_Test::Create(m_pGraphic_Device, strFilePath, m_pScene);
	if (nullptr == m_pLoader || nullptr == m_pScene)
		return E_FAIL;
	if (FAILED(m_pLoader->Ready_Load_Hierachy(m_pScene->GetRootNode())))
		return E_FAIL;

	m_vecMeshData = m_pLoader->GetMeshData();

	return S_OK;
}

void CDynamic_Mesh::Play_Animation(const _float& fTimeDelta)
{

}


void CDynamic_Mesh::Render_Mesh(_matrix matWorld)
{

}

void CDynamic_Mesh::Render_Mesh(_matrix matWorld, ID3D12PipelineState* pPipeLine, 
	ID3D12Resource* pConstantBuffer, CShader* pShader, void* pData, _int iIdx)
{

}

CDynamic_Mesh* CDynamic_Mesh::Create(ID3D12Device* pGraphic_Device, string strFilePath)
{
	CDynamic_Mesh* pInstance = new CDynamic_Mesh(pGraphic_Device);
	if (FAILED(pInstance->Ready_Dynamic_Mesh(strFilePath)))
	{
		MessageBox(0, L"CDynamic_Mesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_Mesh::AnimateFbxNodeHierarchy(FbxTime& fbxCurrentTime)
{

}

CComponent* CDynamic_Mesh::Clone_Component(void* pArg)
{
	return new CDynamic_Mesh(*this);
}

void CDynamic_Mesh::Free()
{
	Safe_Release(m_pLoader);

	CMesh::Free();
}
FbxAMatrix CDynamic_Mesh::ConvertMatrixToFbx(_matrix matWorld)
{
	FbxAMatrix fbxmtxResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{

			fbxmtxResult[i][j] = matWorld.m[i][j];
		}
	}
	return(fbxmtxResult);
}
FbxTime CDynamic_Mesh::Get_CurrentTime()
{

	return 0.f;
}


