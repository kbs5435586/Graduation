#include "stdafx.h"
#include "Dynamic_Mesh.h"
#include "HierarchyLoader.h"
#include "AnimationCtrl.h"


CDynamic_Mesh::CDynamic_Mesh(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh & rhs)
	: CComponent(rhs)
	, m_vecMeshContainer(rhs.m_vecMeshContainer)
	, m_pLoader(rhs.m_pLoader)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_matPivot(rhs.m_matPivot)
	, m_pAnimationCtrl(rhs.m_pAnimationCtrl->Clone())
{
	m_pLoader->AddRef();
}


HRESULT CDynamic_Mesh::Ready_Mesh_Dynamic(const _tchar * pFilePath, const _tchar * pFileName)
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
	
	m_pLoader = CHierarchyLoader::Create(m_pGraphic_Device, pFilePath);
	if (nullptr == m_pLoader)
		return E_FAIL;

	LPD3DXANIMATIONCONTROLLER			pAniCtrl = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphic_Device, m_pLoader, nullptr, &m_pRootFrame, &pAniCtrl)))
		return E_FAIL;

	if (nullptr != pAniCtrl)
	{
		m_pAnimationCtrl = CAnimationCtrl::Create(pAniCtrl);
		if (nullptr == m_pAnimationCtrl)
			return E_FAIL;
	}

	D3DXMatrixRotationY(&m_matPivot, D3DXToRadian(180.f));

	Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matPivot);
	SetUp_CombinedMatrixPointer((D3DXFRAME_DERIVED*)m_pRootFrame);

	return S_OK;
}
void CDynamic_Mesh::Update_Skinning(const _uint & iMeshContainerIdx)
{
	if (m_vecMeshContainer.size() <= iMeshContainerIdx)
		return;

	D3DXMESHCONTAINER_DERIVED*	pMeshContainer = m_vecMeshContainer[iMeshContainerIdx];
	if (nullptr == pMeshContainer)
		return;

	for (size_t i = 0; i < pMeshContainer->dwNumFrames; ++i)
		pMeshContainer->pRenderingMatrices[i] = pMeshContainer->pOffsetMatrices[i] * *pMeshContainer->ppCombinedTransformationMatrices[i];

	void	*pSour, *pDest;

	pMeshContainer->pOriginalMesh->LockVertexBuffer(0, &pSour);
	pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDest);

	// 메시를 뼈에 붙인다.
	pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrices, nullptr, pSour, pDest);

	pMeshContainer->pOriginalMesh->UnlockVertexBuffer();
	pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
}

void CDynamic_Mesh::Render_Mesh(const _uint & iMeshContainerIdx, const _uint & iAttributeID)
{
	if (m_vecMeshContainer.size() <= iMeshContainerIdx ||
		m_vecMeshContainer[iMeshContainerIdx]->NumMaterials <= iAttributeID)
		return;

	D3DXMESHCONTAINER_DERIVED*	pMeshContainer = m_vecMeshContainer[iMeshContainerIdx];
	if (nullptr == pMeshContainer)
		return;

	pMeshContainer->MeshData.pMesh->DrawSubset(iAttributeID);
}
HRESULT CDynamic_Mesh::Set_AnimationSet(const _uint & iAnimationID, ANIMATE_PLAY eID)
{
	return m_pAnimationCtrl->Set_AnimationSet(iAnimationID, eID);
}
HRESULT CDynamic_Mesh::Set_AnimationSet(const _uint & iAnimationID)
{
	return m_pAnimationCtrl->Set_AnimationSet(iAnimationID);
}



void CDynamic_Mesh::Play_Animation(const _float & fTimeDelta)
{
	m_pAnimationCtrl->Play_Animation(fTimeDelta);

	D3DXMatrixRotationY(&m_matPivot, D3DXToRadian(180.f));

	Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matPivot);
}

_bool CDynamic_Mesh::Get_EndLoop()
{
	return m_pAnimationCtrl->Get_EndLoop();
}

_uint CDynamic_Mesh::Get_CurrentState()
{
	return m_pAnimationCtrl->Get_CurrentState();
}

void CDynamic_Mesh::Update_CombinedTransformationMatrices(D3DXFRAME_DERIVED * pFrame, const _matrix * pParentMatrix)
{
	pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix * *pParentMatrix;

	if (nullptr != pFrame->pFrameSibling)
		Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED *)pFrame->pFrameSibling, pParentMatrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED *)pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
}

HRESULT CDynamic_Mesh::SetUp_CombinedMatrixPointer(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		m_vecMeshContainer.push_back(pMeshContainer);

		for (_ulong i = 0; i < pMeshContainer->dwNumFrames; ++i)
		{
			const char* pBoneName = pMeshContainer->pSkinInfo->GetBoneName(i);
			if (nullptr == pBoneName)
				return E_FAIL;

			D3DXFRAME_DERIVED*	pFindFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pBoneName);
			if (nullptr == pFindFrame)
				return E_FAIL;

			pMeshContainer->ppCombinedTransformationMatrices[i] = &pFindFrame->CombinedTransformationMatrix;
		}
	}

	HRESULT			hr = 0;

	if (nullptr != pFrame->pFrameSibling)
	{
		if (FAILED(SetUp_CombinedMatrixPointer((D3DXFRAME_DERIVED *)pFrame->pFrameSibling)))
			return E_FAIL;
	}


	if (nullptr != pFrame->pFrameFirstChild)
	{
		if (FAILED(SetUp_CombinedMatrixPointer((D3DXFRAME_DERIVED *)pFrame->pFrameFirstChild)))
			return E_FAIL;
	}

	return S_OK;
}

CDynamic_Mesh * CDynamic_Mesh::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName)
{
	CDynamic_Mesh*	pInstance = new CDynamic_Mesh(pGraphic_Device);

	if (FAILED(pInstance->Ready_Mesh_Dynamic(pFilePath, pFileName)))
	{
		MessageBox(0, L"CMesh_Dynamic Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CDynamic_Mesh::Clone_Component(void* pArg)
{
	return new CDynamic_Mesh(*this);
}

void CDynamic_Mesh::Free()
{
	if (false == m_isClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);

		for (auto& pMeshContainer : m_vecMeshContainer)
		{
			m_pLoader->DestroyMeshContainer(pMeshContainer);
		}
	}
	m_vecMeshContainer.clear();

	Safe_Release(m_pAnimationCtrl);
	Safe_Release(m_pLoader);

	CComponent::Free();
}
