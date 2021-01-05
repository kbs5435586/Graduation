#include "stdafx.h"
#include "Static_Mesh.h"
#include "Transform.h"
#include "Picking.h"

CStatic_Mesh::CStatic_Mesh(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CStatic_Mesh::CStatic_Mesh(const CStatic_Mesh & rhs)
	: CComponent(rhs)
	, m_pMesh(rhs.m_pMesh)
	, m_dwNumMaterials(rhs.m_dwNumMaterials)
	, m_pSubSetDesc(rhs.m_pSubSetDesc)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
	, m_vTempMin(rhs.m_vTempMin)
	, m_vTempMax(rhs.m_vTempMax)
	, m_pPosition(rhs.m_pPosition)
{
	for (size_t i = 0; i < m_dwNumMaterials; i++)
	{
		if (nullptr != m_pSubSetDesc[i].MeshTexture.pDiffuseTexture)
			m_pSubSetDesc[i].MeshTexture.pDiffuseTexture->AddRef();
		if (nullptr != m_pSubSetDesc[i].MeshTexture.pNormalTexture)
			m_pSubSetDesc[i].MeshTexture.pNormalTexture->AddRef();
		if (nullptr != m_pSubSetDesc[i].MeshTexture.pSpecularTexture)
			m_pSubSetDesc[i].MeshTexture.pSpecularTexture->AddRef();
	}

	m_pMesh->AddRef();
}

HRESULT CStatic_Mesh::Ready_Mesh_Static(const _tchar * pFilePath, const _tchar * pFileName)
{
	LPD3DXMESH		pMesh = nullptr;

	_tchar			szFullPath[MAX_PATH] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	int iLength = lstrlen(pFileName);
	m_pGameObjectName = new _tchar[iLength + 12];
	m_pObject_FileName = new _tchar[iLength];

	lstrcpy(m_pGameObjectName, L"GameObject_");
	lstrcat(m_pGameObjectName, pFileName);
	lstrcpy(m_pObject_FileName, pFileName);

	m_pGameObjectName = wcstok(m_pGameObjectName, L".");
	m_pObject_FileName = wcstok(m_pObject_FileName, L".");

	if (FAILED(D3DXLoadMeshFromX(szFullPath, 0, m_pGraphic_Device, &m_pAdjacency, &m_pMaterials, nullptr, &m_dwNumMaterials, &pMesh)))
		return E_FAIL;

	m_pSubSetDesc = new SUBSETDESC[m_dwNumMaterials];
	ZeroMemory(m_pSubSetDesc, sizeof(SUBSETDESC) * m_dwNumMaterials);

	for (size_t i = 0; i < m_dwNumMaterials; i++)
	{
		D3DXMATERIAL* pMaterial = (D3DXMATERIAL*)m_pMaterials->GetBufferPointer() + i;

		if(pMaterial->pTextureFilename==nullptr)
			continue;

		m_pSubSetDesc[i].Material = *pMaterial;

		ZeroMemory(szFullPath, sizeof(_tchar) * MAX_PATH);

		_tchar			szFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, pMaterial->pTextureFilename, strlen(pMaterial->pTextureFilename), szFileName, MAX_PATH);


		lstrcpy(szFullPath, pFilePath);
		lstrcat(szFullPath, szFileName);

		_tchar		szTmp[MAX_PATH] = L"";
		lstrcpy(szTmp, szFullPath);


		if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szTmp, &m_pSubSetDesc[i].MeshTexture.pDiffuseTexture)))
			return E_FAIL;

		lstrcpy(szTmp, szFullPath);
		if (SUCCEEDED(Change_TextureFileName(szTmp, L"D", L"N")))
		{
			D3DXCreateTextureFromFile(m_pGraphic_Device, szTmp, &m_pSubSetDesc[i].MeshTexture.pNormalTexture);
		}

		lstrcpy(szTmp, szFullPath);
		if (SUCCEEDED(Change_TextureFileName(szTmp, L"D", L"S")))
		{
			D3DXCreateTextureFromFile(m_pGraphic_Device, szTmp, &m_pSubSetDesc[i].MeshTexture.pSpecularTexture);
		}


	}

	_ulong		dwFVF = pMesh->GetFVF();


	if (false == (dwFVF & D3DFVF_NORMAL))
	{
		if (FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphic_Device, &m_pMesh)))
			return E_FAIL;

		if (dwFVF == 0)
			_int i = 0;
		else if (FAILED(D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer())))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, m_pGraphic_Device, &m_pMesh)))
			return E_FAIL;
	}

	Safe_Release(pMesh);

	D3DVERTEXELEMENT9			Element[MAX_FVF_DECL_SIZE];
	ZeroMemory(Element, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	_ushort	wOffset = 0;

	m_pMesh->GetDeclaration(Element);

	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_POSITION == Element[i].Usage)
		{
			wOffset = Element[i].Offset;
			break;
		}
	}


	void*			pVertices = nullptr;

	m_pMesh->LockVertexBuffer(0, &pVertices);

	if (FAILED(D3DXComputeBoundingBox(((_vec3*)((_byte*)pVertices) + wOffset), m_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pMesh->GetFVF()), &m_vMin, &m_vMax)))
		return E_FAIL;

	m_vTempMax = m_vMax;
	m_vTempMin = m_vMin;

	m_pPosition = new _vec3[8];
	VTXCUBETEX	pVertice[8] = {};
	pVertice[0].vPosition;

	pVertice[0].vPosition = _vec3(m_vTempMin.x, m_vTempMax.y, m_vTempMin.z);
	pVertice[1].vPosition = _vec3(m_vTempMax.x, m_vTempMax.y, m_vTempMin.x);
	pVertice[2].vPosition = _vec3(m_vTempMax.x, m_vTempMin.y, m_vTempMin.x);
	pVertice[3].vPosition = _vec3(m_vTempMin.x, m_vTempMin.y, m_vTempMin.x);

	pVertice[4].vPosition = _vec3(m_vTempMin.x, m_vTempMax.y, m_vTempMax.z);
	pVertice[5].vPosition = _vec3(m_vTempMax.x, m_vTempMax.y, m_vTempMax.z);
	pVertice[6].vPosition = _vec3(m_vTempMax.x, m_vTempMin.y, m_vTempMax.z);
	pVertice[7].vPosition = _vec3(m_vTempMin.x, m_vTempMin.y, m_vTempMax.z);

	for (size_t i = 0; i < 8; ++i)
		m_pPosition[i] = pVertice[i].vPosition;

	m_pMesh->UnlockVertexBuffer();

	return S_OK;
}

void CStatic_Mesh::Render_Mesh(_uint iAttributeID)
{
	if (nullptr == m_pMesh)
		return;
	m_pMesh->DrawSubset(iAttributeID);
}

CStatic_Mesh * CStatic_Mesh::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName)
{
	CStatic_Mesh*	pInstance = new CStatic_Mesh(pGraphic_Device);

	if (FAILED(pInstance->Ready_Mesh_Static(pFilePath, pFileName)))
	{
		MessageBox(0, L"CMesh_Static Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CStatic_Mesh::Clone_Component(void* pArg)
{
	return new CStatic_Mesh(*this);
}

void CStatic_Mesh::Free()
{
	for (size_t i = 0; i < m_dwNumMaterials; i++)
	{
		Safe_Release(m_pSubSetDesc[i].MeshTexture.pDiffuseTexture);
		Safe_Release(m_pSubSetDesc[i].MeshTexture.pNormalTexture);
		Safe_Release(m_pSubSetDesc[i].MeshTexture.pSpecularTexture);
	}

	if (false == m_isClone)
	{
		Safe_Delete_Array(m_pSubSetDesc);
		Safe_Delete_Array(m_pGameObjectName);
	}

	Safe_Release(m_pMesh);

	CComponent::Free();
}

HRESULT CStatic_Mesh::Change_TextureFileName(_tchar * pFullPath, const _tchar * pSour, const _tchar * pDest)
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

	return S_OK;
}

_bool CStatic_Mesh::Picking_ToMesh(_vec3 * pOut, const CTransform * pTransformCom, const CPicking * pPickingCom)
{
	_vec3	vRayPivot = pPickingCom->Get_MouseRayPivot();
	_vec3	vRay = pPickingCom->Get_MouseRay();

	_matrix		matWorldInv = pTransformCom->Get_Matrix_Inverse();

	D3DXVec3TransformCoord(&vRayPivot, &vRayPivot, &matWorldInv);
	D3DXVec3TransformNormal(&vRay, &vRay, &matWorldInv);
	

	_float fU, fV, fDist;
	bool	isPick = false;
	//Front
	if (D3DXIntersectTri(&m_pPosition[1], &m_pPosition[0], &m_pPosition[2], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[3], &m_pPosition[2], &m_pPosition[0], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	//Up
	if (D3DXIntersectTri(&m_pPosition[5], &m_pPosition[4], &m_pPosition[1], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[0], &m_pPosition[1], &m_pPosition[4], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	//Back
	if (D3DXIntersectTri(&m_pPosition[4], &m_pPosition[5], &m_pPosition[7], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[6], &m_pPosition[7], &m_pPosition[5], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	//Donw
	if (D3DXIntersectTri(&m_pPosition[2], &m_pPosition[3], &m_pPosition[6], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[7], &m_pPosition[6], &m_pPosition[3], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	//Right
	if (D3DXIntersectTri(&m_pPosition[5], &m_pPosition[1], &m_pPosition[6], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[2], &m_pPosition[6], &m_pPosition[1], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	//Left
	if (D3DXIntersectTri(&m_pPosition[0], &m_pPosition[4], &m_pPosition[3], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[7], &m_pPosition[3], &m_pPosition[4], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;


	return isPick;
}
