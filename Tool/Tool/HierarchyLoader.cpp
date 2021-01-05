#include "stdafx.h"
#include "HierarchyLoader.h"

CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CHierarchyLoader::Ready_HierarchyLoader(const _tchar * pFilePath)
{
	m_pFilePath = pFilePath;

	return NOERROR;
}

STDMETHODIMP CHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pFrame, sizeof(D3DXFRAME_DERIVED));

	Allocate_Name(Name, &pFrame->Name);

	pFrame->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	*ppNewFrame = pFrame;

	return NOERROR;
}


// 로드한 메시를 보관하기위한 공간을 할당한다.
STDMETHODIMP CHierarchyLoader::CreateMeshContainer(LPCSTR Name
	, CONST D3DXMESHDATA * pMeshData
	, CONST D3DXMATERIAL * pMaterials
	, CONST D3DXEFFECTINSTANCE * pEffectInstances
	, DWORD NumMaterials
	, CONST DWORD * pAdjacency
	, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*		pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	Allocate_Name(Name, &pMeshContainer->Name);

	
	
	// 실제 사용되는 메시의 타입을 보관한다.
	pMeshContainer->MeshData.Type = pMeshData->Type;

	// 실제 사용할 메시를 저장한다.
	LPD3DXMESH	pMesh = pMeshData->pMesh;
	pMesh->AddRef();

	// 인접폴리곤인덱스를 보관한다.
	_ulong		dwNumPolygons = pMesh->GetNumFaces();

	pMeshContainer->pAdjacency = new _ulong[dwNumPolygons * 3];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumPolygons * 3);

	_ulong dwMeshFVF = pMesh->GetFVF();

	if (dwMeshFVF & D3DFVF_NORMAL)
	{
		if (FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF, m_pGraphic_Device, &pMeshContainer->MeshData.pMesh)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF | D3DFVF_NORMAL, m_pGraphic_Device, &pMeshContainer->MeshData.pMesh)))
			return E_FAIL;

		if (FAILED(D3DXComputeNormals(pMeshContainer->MeshData.pMesh, pMeshContainer->pAdjacency)))
			return E_FAIL;
	}

	Safe_Release(pMesh);

	// 탄젠트를 추가한다.
	
	// 서브셋의 갯수를 저장한다.
	pMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;	

	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];

	// 서브셋 정보를 저장한다.
	if (0 == NumMaterials)
	{
		pMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Power = 0.f;
		pMeshContainer->pMaterials[0].pTextureFilename = nullptr;
	}
	else			
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pMeshContainer->NumMaterials);	

	pMeshContainer->pSubSetDesc = new SUBSETDESC[pMeshContainer->NumMaterials];
	ZeroMemory(pMeshContainer->pSubSetDesc, sizeof(SUBSETDESC) * pMeshContainer->NumMaterials);

	for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		pMeshContainer->pSubSetDesc[i].Material = pMeshContainer->pMaterials[i];	

		_tchar			szFullPath[MAX_PATH] = L"";
		_tchar			szFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, pMeshContainer->pMaterials[i].pTextureFilename, strlen(pMeshContainer->pMaterials[i].pTextureFilename), szFileName, MAX_PATH);

		lstrcpy(szFullPath, m_pFilePath);
		lstrcat(szFullPath, szFileName);

		_tchar		szTmp[MAX_PATH] = L"";

		lstrcpy(szTmp, szFullPath);
		if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szTmp, &pMeshContainer->pSubSetDesc[i].MeshTexture.pDiffuseTexture)))
			return E_FAIL;

		lstrcpy(szTmp, szFullPath);
		if (SUCCEEDED(Change_TextureFileName(szTmp, L"D", L"N")))
		{
			D3DXCreateTextureFromFile(m_pGraphic_Device, szTmp, &pMeshContainer->pSubSetDesc[i].MeshTexture.pNormalTexture);
		}

		lstrcpy(szTmp, szFullPath);
		if (SUCCEEDED(Change_TextureFileName(szTmp, L"D", L"S")))
		{
			D3DXCreateTextureFromFile(m_pGraphic_Device, szTmp, &pMeshContainer->pSubSetDesc[i].MeshTexture.pSpecularTexture);
		}	
	}

	// 피부에 대한 정보를 저장한다.
	pMeshContainer->pSkinInfo = pSkinInfo;
	pMeshContainer->pSkinInfo->AddRef();

	pMeshContainer->dwNumFrames = pMeshContainer->pSkinInfo->GetNumBones();

	pMeshContainer->pOffsetMatrices = new D3DXMATRIX[pMeshContainer->dwNumFrames];

	for (_ulong i = 0; i < pMeshContainer->dwNumFrames; ++i)	
		pMeshContainer->pOffsetMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	pMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pMeshContainer->dwNumFrames];
	pMeshContainer->pRenderingMatrices = new D3DXMATRIX[pMeshContainer->dwNumFrames];	


	if (FAILED(pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), pMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphic_Device, &pMeshContainer->pOriginalMesh)))
		return E_FAIL;

	*ppNewMeshContainer = pMeshContainer;

	return NOERROR;
}

HRESULT CHierarchyLoader::Change_TextureFileName(_tchar* pFullPath, const _tchar * pSour, const _tchar * pDest)
{
	size_t	iLength = lstrlen(pFullPath);

	for (size_t i = iLength; i >= 0; --i)
	{

		if (pFullPath[i] == *pSour)
		{
			pFullPath[i] = *pDest;
			break;
		}

		if (i == 0)
			return E_FAIL;
	}

	return NOERROR;
}

STDMETHODIMP CHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);


	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Safe_Delete(pFrameToFree);

	return NOERROR;
}

STDMETHODIMP CHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);
	Safe_Release(pMeshContainer->pSkinInfo);
	Safe_Release(pMeshContainer->pOriginalMesh);	

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		Safe_Release(pMeshContainer->pSubSetDesc[i].MeshTexture.pDiffuseTexture);
		Safe_Release(pMeshContainer->pSubSetDesc[i].MeshTexture.pNormalTexture);
		Safe_Release(pMeshContainer->pSubSetDesc[i].MeshTexture.pSpecularTexture);
	}

	Safe_Delete_Array(pMeshContainer->pSubSetDesc);
	Safe_Delete_Array(pMeshContainer->ppCombinedTransformationMatrices);
	Safe_Delete_Array(pMeshContainer->pOffsetMatrices);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrices);



	Safe_Delete(pMeshContainer);

	return NOERROR;
}

_ulong CHierarchyLoader::AddRef()
{
	return ++m_dwRefCnt;
}

_ulong CHierarchyLoader::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}

	else
		return m_dwRefCnt--;
}

HRESULT CHierarchyLoader::Allocate_Name(const char * pSourName, char ** ppDestName)
{
	if (nullptr == pSourName)
		return E_FAIL;

	size_t iLength = strlen(pSourName);

	*ppDestName = new char[iLength + 1];
	ZeroMemory(*ppDestName, sizeof(char) * iLength + 1);

	strcpy(*ppDestName, pSourName);

	return NOERROR;
}

CHierarchyLoader * CHierarchyLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath)
{
	CHierarchyLoader*	pInstance = new CHierarchyLoader(pGraphic_Device);

	if (FAILED(pInstance->Ready_HierarchyLoader(pFilePath)))
	{
		MessageBox(0, L"CHierarchyLoader Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHierarchyLoader::Free()
{
	Safe_Release(m_pGraphic_Device);
}
