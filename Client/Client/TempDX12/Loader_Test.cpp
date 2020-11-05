#include "framework.h"
#include "Loader_Test.h"

CLoader_Test::CLoader_Test(ID3D12Device* pGraphic_Device)
{
}

HRESULT CLoader_Test::Ready_Hierachy_Loader(string strFilePath, FbxScene*& pScene)
{
	_int iSDKMajor;
	_int iSDKMinor;
	_int iSDKRevision;

	_int iFileFormatMajor;
	_int iFileFormatMinor;
	_int iFileFormatRevision;

	FbxManager::GetFileFormatVersion(iSDKMajor, iSDKMinor, iSDKRevision);
	FbxImporter* pFbxImporter = FbxImporter::Create(g_FbxManager, "");

	if (nullptr == pFbxImporter)
		return E_FAIL;

	if (!pFbxImporter->Initialize(strFilePath.c_str(), -1, g_FbxManager->GetIOSettings()))
	{
		FbxStatus eStatus = pFbxImporter->GetStatus();
		assert(nullptr);
	}
	
	pFbxImporter->GetFileVersion(iFileFormatMajor, iFileFormatMinor, iFileFormatRevision);

	pScene = FbxScene::Create(g_FbxManager, "");
	pFbxImporter->Import(pScene);

	FbxGeometryConverter		FbxGeomConverter(g_FbxManager);
	FbxGeomConverter.Triangulate(pScene, TRUE);

	pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	pFbxImporter->Destroy();

	return S_OK;
}

HRESULT CLoader_Test::Ready_Load_Hierachy(FbxNode* pNode)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	if (!(pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType()))
		return E_FAIL;
	else
	{
		FbxMesh* pMesh = pNode->GetMesh();
		if (nullptr == pMesh)
			return E_FAIL;

		_int	iVertices = pMesh->GetControlPointsCount();
		_int	iIndices = 0;
		_int	iPolygons = pMesh->GetPolygonCount();

		for (_uint i = 0; i < iPolygons; ++i)
			iIndices += pMesh->GetPolygonSize(i);

		_int* pIndices = new _int[iIndices];
		for (_uint i = 0, k = 0; i < iPolygons; ++i)
		{
			_int iPolygonSize = pMesh->GetPolygonSize(i);
			for (_uint j = 0; j < iPolygonSize; ++j)
				pIndices[k++] = pMesh->GetPolygonVertex(i, j);
		}

		MeshData* pData = new MeshData(iVertices, iIndices, pIndices);
		m_vecMeshData.push_back(pData);

	}
	return S_OK;
}


CLoader_Test* CLoader_Test::Create(ID3D12Device* pGraphic_Device, string strFilePath, FbxScene*& pScene)
{
	CLoader_Test* pInstance = new CLoader_Test(pGraphic_Device);
	if (FAILED(pInstance->Ready_Hierachy_Loader(strFilePath, pScene)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CLoader_Test::Free()
{
}
