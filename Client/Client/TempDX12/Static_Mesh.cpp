#include "framework.h"
#include "Static_Mesh.h"
#include "Hierachy_Loader.h"
#include "Shader.h"
#include "Texture.h"

CStatic_Mesh::CStatic_Mesh(ID3D12Device* pGraphic_Device)
	: CMesh(pGraphic_Device)
{
}

CStatic_Mesh::CStatic_Mesh(const CStatic_Mesh& rhs)
	: CMesh(rhs)
	, m_pLoader(rhs.m_pLoader)
{
	m_pLoader->AddRef();
	m_IsClone = true;
}


HRESULT CStatic_Mesh::Ready_Static_Mesh(string strFilePath)
{
	m_pLoader = CHierachy_Loader::Create(m_pGraphic_Device, strFilePath, m_pScene);
	if (nullptr == m_pLoader || nullptr == m_pScene)
		return E_FAIL;
	if (FAILED(m_pLoader->Ready_Load_Hierachy(m_pScene->GetRootNode())))
		return E_FAIL;

	_int i = 0;
	return S_OK;
}


void CStatic_Mesh::Render_HierachyLoader(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxNode* pNode, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType)
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

void CStatic_Mesh::Render_HierachyLoader(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxNode* pNode, _matrix matWorld, MAINPASS_LIGHT tPass, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxAMatrix RootNodeMatrix = pNode->EvaluateGlobalTransform();
		FbxAMatrix GeometicOffest = GetGeometricOffsetTransform(pNode);

		FbxMesh* pfbxMesh = pNode->GetMesh();
		Render_Buffer(pPipeLine, pShader, pfbxMesh, RootNodeMatrix, GeometicOffest, matWorld, tPass, iPassSize, pData, eType);
	}

	_uint iChildCnt = pNode->GetChildCount();
	for (_uint i = 0; i < iChildCnt; ++i)
	{
		Render_HierachyLoader(pPipeLine, pShader, pNode->GetChild(i), matWorld, tPass, iPassSize, pData, eType);
	}
}


void CStatic_Mesh::Render_Mesh(ID3D12PipelineState* pPipeLine, CShader* pShader, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	if (nullptr != pShader && pPipeLine != nullptr)
		Render_HierachyLoader(pPipeLine, pShader, m_pScene->GetRootNode(), matWorld, iPassSize, pData, eType);
}

void CStatic_Mesh::Render_Mesh(ID3D12PipelineState* pPipeLine, CShader* pShader, _matrix matWorld, MAINPASS_LIGHT tPass, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	if (nullptr != pShader && pPipeLine != nullptr)
		Render_HierachyLoader(pPipeLine, pShader, m_pScene->GetRootNode(), matWorld, tPass, iPassSize, pData, eType);
}




FbxAMatrix CStatic_Mesh::ConvertMatrixToFbx(_matrix matWorld)
{
	FbxAMatrix fbxmtxResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxResult[i][j] = matWorld.m[i][j];
	}
	return(fbxmtxResult);
}


void CStatic_Mesh::Render_Buffer(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxMesh* pMesh, FbxAMatrix& pFbxRootNodeMatrix, 
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
	CDevice::GetInstance()->GetCommandList()->DrawInstanced(pInfo->vecMeshData.size(), 1, 0, 0);
 }

void CStatic_Mesh::Render_Buffer(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxMesh* pMesh, FbxAMatrix& pFbxRootNodeMatrix, 
	FbxAMatrix& pGeomatryMatrix, _matrix matWorld, MAINPASS_LIGHT tPass, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	FbxAMatrix	FbxMatrixTransform = ConvertMatrixToFbx(matWorld);
	_int		iSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iSkinDeformers == 0)
		FbxMatrixTransform = ConvertMatrixToFbx(matWorld) * pFbxRootNodeMatrix * pGeomatryMatrix;

	if (FAILED(pShader->SetUp_OnShader(pPipeLine, &FbxMatrixTransform, CDevice::GetInstance()->GetViewMatrix(), 
		CDevice::GetInstance()->GetProjectionMatrix(), tPass, iPassSize, pData, eType)))
		return;

	RenderInfo* pInfo = (RenderInfo*)pMesh->GetUserDataPtr();


	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &(pInfo->VertexBufferView));
	CDevice::GetInstance()->GetCommandList()->DrawInstanced(pInfo->vecMeshData.size(), 1, 0, 0);
}

FbxAMatrix CStatic_Mesh::GetGeometricOffsetTransform(FbxNode* pNode)
{
	const FbxVector4 T = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}


CStatic_Mesh* CStatic_Mesh::Create(ID3D12Device* pGraphic_Device, string strFilePath)
{
	CStatic_Mesh* pInstance = new CStatic_Mesh(pGraphic_Device);
	if (FAILED(pInstance->Ready_Static_Mesh(strFilePath)))
	{
		MessageBox(0, L"CStatic_Mesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CStatic_Mesh::Clone_Component(void* pArg)
{
	return new CStatic_Mesh(*this);
}

void CStatic_Mesh::Free()
{
	Safe_Release(m_pLoader);

	CMesh::Free();
}
