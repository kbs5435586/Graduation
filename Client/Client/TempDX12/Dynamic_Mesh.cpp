#include "framework.h"
#include "Dynamic_Mesh.h"
#include "Hierachy_Loader.h"
#include "Animation_Controller.h"

CDynamic_Mesh::CDynamic_Mesh(ID3D12Device* pGraphic_Device)
	: CMesh(pGraphic_Device)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh& rhs)
	: CMesh(rhs)
{
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

	FbxGeometryConverter		FbxGeomConverter(g_FbxManager);
	FbxGeomConverter.Triangulate(m_pFbxScene, TRUE);

	FbxSystemUnit				FbxSceneSystemUnit = m_pFbxScene->GetGlobalSettings().GetSystemUnit();
	if (FbxSceneSystemUnit.GetScaleFactor() != 1.f)
		FbxSystemUnit::cm.ConvertScene(m_pFbxScene);

	pFbxImporter->Destroy();
	m_pLoader = CHierachy_Loader::Create(m_pGraphic_Device,m_pFbxScene);
	if (m_pLoader == nullptr)
		return E_FAIL;
	m_pController = CAnimation_Controller::Create(m_pFbxScene);
	if (nullptr == m_pController)
		return E_FAIL;


	return S_OK;
}

void CDynamic_Mesh::Play_Animation(const _float& fTimeDelta)
{
	if (nullptr == m_pController)
		return;
	m_pController->Play_Animation(fTimeDelta);
	m_fTime = fTimeDelta;
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

void CDynamic_Mesh::Render_Mesh(_matrix matWorld)
{
	if (nullptr != m_pLoader && nullptr != m_pFbxScene)
	{
		m_pLoader->Render_Hierachy_Mesh(m_pFbxScene->GetRootNode(), ConvertMatrixToFbx(matWorld), m_fTime);
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

void CDynamic_Mesh::Free()
{
	if (m_pLoader)
		m_pLoader->Release();
	CMesh::Free();
}