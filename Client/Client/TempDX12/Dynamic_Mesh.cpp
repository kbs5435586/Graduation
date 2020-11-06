#include "framework.h"
#include "Dynamic_Mesh.h"
#include "Animation_Controller.h"
#include "Loader_Test.h"
#include "Shader.h"

CDynamic_Mesh::CDynamic_Mesh(ID3D12Device* pGraphic_Device)
	: CMesh(pGraphic_Device)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh& rhs)
	: CMesh(rhs)
	, m_pLoader(rhs.m_pLoader)
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

	return S_OK;
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

<<<<<<< HEAD
=======
	if(m_pController!=nullptr)
	m_pController->Set_Animation(m_pFbxScene, 0);
>>>>>>> parent of fb04460... Animation -ing

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
		for (int j = 0; j < 4; j++) fbxmtxResult[i][j] = matWorld.m[i][j];
	}
	return(fbxmtxResult);
}

void CDynamic_Mesh::Render_HierachyLoader(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxNode* pNode, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxAMatrix RootNodeMatrix = pNode->EvaluateGlobalTransform();
		FbxAMatrix GeometicOffest = GetGeometricOffsetTransform(pNode);

		FbxMesh* pfbxMesh = pNode->GetMesh();
		Render_Buffer(pPipeLine, pShader, pfbxMesh, RootNodeMatrix, GeometicOffest, matWorld, iPassSize, pData, eType);
	}

	_uint iChildCnt = pNode->GetChildCount();
	for (_uint i = 0; i < iChildCnt; ++i)
	{
		Render_HierachyLoader(pPipeLine, pShader, pNode->GetChild(i), matWorld, iPassSize, pData, eType);
	}

}
<<<<<<< HEAD
void CDynamic_Mesh::Render_Buffer(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxMesh* pMesh, FbxAMatrix& pFbxRootNodeMatrix,
	FbxAMatrix& pGeomatryMatrix, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	FbxAMatrix	FbxMatrixTransform = ConvertMatrixToFbx(matWorld);
	_int		iSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iSkinDeformers == 0)
		FbxMatrixTransform = ConvertMatrixToFbx(matWorld) * pFbxRootNodeMatrix * pGeomatryMatrix;

	if (FAILED(pShader->SetUp_OnShader(pPipeLine, &FbxMatrixTransform,
		CDevice::GetInstance()->GetViewMatrix(), CDevice::GetInstance()->GetProjectionMatrix(),
		iPassSize, pData, eType)))
		return;
	RenderInfo* pInfo = (RenderInfo*)pMesh->GetUserDataPtr();

	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &(pInfo->VertexBufferView));
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&(pInfo->IndexBufferView));
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(pInfo->iIndices, 1, 0, 0, 0);
}
FbxAMatrix CDynamic_Mesh::GetGeometricOffsetTransform(FbxNode* pNode)
{
	const FbxVector4 T = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}
void CDynamic_Mesh::Render_Mesh(ID3D12PipelineState* pPipeLine, CShader* pShader, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	if (nullptr != pShader && pPipeLine != nullptr)
		Render_HierachyLoader(pPipeLine, pShader, m_pScene->GetRootNode(), matWorld, iPassSize, pData, eType);

}

=======

void CDynamic_Mesh::Free()
{
	Safe_Release(m_pLoader);
	Safe_Release(m_pController);
>>>>>>> parent of fb04460... Animation -ing

