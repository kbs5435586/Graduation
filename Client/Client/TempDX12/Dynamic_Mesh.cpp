#include "framework.h"
#include "Dynamic_Mesh.h"
#include "Animation_Controller.h"
#include "Hierachy_Loader.h"

CDynamic_Mesh::CDynamic_Mesh(ID3D12Device* pGraphic_Device)
	: CMesh(pGraphic_Device)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh& rhs)
	: CMesh(rhs)
	, m_pLoader(rhs.m_pLoader)
	/*, m_pController(rhs.m_pController)*/
{
	m_pLoader->AddRef();
	//m_pController->AddRef();
	m_IsClone = true;
}

HRESULT CDynamic_Mesh::Ready_Dynamic_Mesh(string strFilePath)
{
	_int iSDKMajor, iSDKMinor, iSDKRevision;
	FbxManager::GetFileFormatVersion(iSDKMajor, iSDKMinor, iSDKRevision);
	FbxImporter* pFbxImporter = FbxImporter::Create(g_FbxManager, "");

	if (nullptr == pFbxImporter)
		return E_FAIL;

	_bool		IsImportStatus = pFbxImporter->Initialize(strFilePath.c_str(), -1, g_FbxManager->GetIOSettings());
	_int		iFileFormatMajor, iFileFormatMinor, iFileFormatRevision;
	pFbxImporter->GetFileVersion(iFileFormatMajor, iFileFormatMinor, iFileFormatRevision);

	m_pFbxScene = FbxScene::Create(g_FbxManager, "");
	_bool	IsStatus = pFbxImporter->Import(m_pFbxScene);
	m_pFbxScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::DirectX);
	FbxGeometryConverter		FbxGeomConverter(g_FbxManager);
	FbxGeomConverter.Triangulate(m_pFbxScene, TRUE);

	FbxSystemUnit				FbxSceneSystemUnit = m_pFbxScene->GetGlobalSettings().GetSystemUnit();


	FbxSystemUnit fbxSceneSystemUnit = m_pFbxScene->GetGlobalSettings().GetSystemUnit();
	if (FbxSceneSystemUnit.GetScaleFactor() != 1.f)
		FbxSystemUnit::cm.ConvertScene(m_pFbxScene);

	pFbxImporter->Destroy();

	

	return S_OK;
}

void CDynamic_Mesh::Play_Animation(const _float& fTimeDelta)
{
	/*if (nullptr == m_pController)
		return;
	m_pController->Play_Animation(fTimeDelta);*/
	m_fTime = fTimeDelta;
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

void CDynamic_Mesh::Render_Mesh(_matrix matWorld)
{
	//if (nullptr != m_pLoader && nullptr != m_pFbxScene)
	//{
	//	m_pLoader->Render_Hierachy_Mesh(m_pFbxScene->GetRootNode(), ConvertMatrixToFbx(matWorld), m_fTime);
	//}
}

void CDynamic_Mesh::Render_Mesh(_matrix matWorld, ID3D12PipelineState* pPipeLine, ID3D12Resource* pConstantBuffer, CShader* pShader, void* pData, _int iIdx)
{
	if (nullptr != m_pLoader && nullptr != m_pFbxScene)
	{
		//m_pLoader->Render_Hierachy_Mesh(m_pFbxScene->GetRootNode(), ConvertMatrixToFbx(matWorld), m_fTime, pPipeLine, pConstantBuffer, pShader, pData, iIdx);
	}
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

CComponent* CDynamic_Mesh::Clone_Component(void* pArg)
{
	return new CDynamic_Mesh(*this);
}

void CDynamic_Mesh::AnimateFbxNodeHierarchy(FbxTime& fbxCurrentTime)
{
	/*if (nullptr == m_pLoader)
		return;
	return m_pLoader->AnimateFbxNodeHierarchy(m_pFbxScene->GetRootNode(), fbxCurrentTime);*/
}


FbxTime CDynamic_Mesh::Get_CurrentTime()
{
	/*if (nullptr == m_pController)
		return -1;
	return m_pController->Get_CurrentTime();*/
	return 0.f;
}



void CDynamic_Mesh::Free()
{
	Safe_Release(m_pLoader);
	//Safe_Release(m_pController);

	CMesh::Free();
}