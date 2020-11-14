#include "framework.h"
#include "Animation_Controller.h"

CAnimation_Controller::CAnimation_Controller(ID3D12Device* pGraphic_Device)
{
}

HRESULT CAnimation_Controller::Ready_Animation_Controller(FbxScene* pScene)
{
	FbxArray<FbxString*>		ArrFbxAnimationStackName;
	pScene->FillAnimStackNameArray(ArrFbxAnimationStackName);

	m_iAnimationStack = ArrFbxAnimationStackName.Size();

	m_pFbxAnimStack =	new FbxAnimStack*[m_iAnimationStack];
	m_pFbxStartTime =	new FbxTime[m_iAnimationStack];
	m_pFbxStopTime =	new FbxTime[m_iAnimationStack];
	m_pFbxCurrTime =	new FbxTime[m_iAnimationStack];

	for (_uint i = 0; i < m_iAnimationStack; ++ i)
	{
		FbxString* pFbxStackName = ArrFbxAnimationStackName[i];
		FbxAnimStack* pAnimStack = pScene->FindMember<FbxAnimStack>(pFbxStackName->Buffer());

		m_pFbxAnimStack[i] = pAnimStack;

		FbxTakeInfo* pInfo = pScene->GetTakeInfo(*pFbxStackName);
		FbxTime fbxStartTime, fbxStopTime;
		if (pInfo)
		{
			fbxStartTime = pInfo->mLocalTimeSpan.GetStart();
			fbxStopTime = pInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan fbxTimeLineTimeSpan;

			pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
			fbxStartTime = fbxTimeLineTimeSpan.GetStart();
			fbxStopTime = fbxTimeLineTimeSpan.GetStop();
		}
		
		m_pFbxStartTime[i] =	fbxStartTime;
		m_pFbxStopTime[i] =		fbxStopTime;

		m_pFbxCurrTime[i] = FbxTime(0);

	}
	FbxArrayDelete(ArrFbxAnimationStackName);
	return S_OK;
}

CAnimation_Controller* CAnimation_Controller::Create(ID3D12Device* pGraphic_Device, FbxScene* pScene)
{
	CAnimation_Controller* pInstance = new CAnimation_Controller(pGraphic_Device);
	if (FAILED(pInstance->Ready_Animation_Controller(pScene)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CAnimation_Controller::Free()
{
	Safe_Delete_Array(m_pFbxAnimStack);
	Safe_Delete_Array(m_pFbxCurrTime);
	Safe_Delete_Array(m_pFbxStopTime);
	Safe_Delete_Array(m_pFbxStartTime);

}

void CAnimation_Controller::SetPosition(_int iAnimStack, _float fPosition)
{
	m_pFbxCurrTime[iAnimStack].SetSecondDouble(fPosition);
}

void CAnimation_Controller::SetAnimationStack(FbxScene* pScene, _int iAnimStack)
{
	m_iAnimationStack = iAnimStack;
	pScene->SetCurrentAnimationStack(m_pFbxAnimStack[m_iAnimationStack]);
}

void CAnimation_Controller::AdvacneTime(const _float& fTimeDelta)
{
	FbxTime fbxTime;
	fbxTime.SetSecondDouble(fTimeDelta);

	m_pFbxCurrTime[m_iAnimationStack] += fbxTime;
	if (m_pFbxCurrTime[m_iAnimationStack] > m_pFbxStopTime[m_iAnimationStack])
		m_pFbxCurrTime[m_iAnimationStack] = m_pFbxStartTime[m_iAnimationStack];
}

void CAnimation_Controller::AnimateMesh(FbxMesh* pMesh, FbxTime& fbxCurTime)
{
	_uint iVertices = pMesh->GetControlPointsCount();
	if (iVertices > 0)
	{
		FbxVector4* pVertices = new FbxVector4[iVertices];
		memcpy(pVertices, pMesh->GetControlPoints(), iVertices * sizeof(FbxVector4));

		_int iSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (iSkinDeformers > 0)
		{

		}

		RenderInfo* pInfo = (RenderInfo*)pMesh->GetUserDataPtr();
		for (_uint i = 0; i < iVertices; ++i)
		{
			pInfo->vecPosition[i] = _vec3(pVertices[i][0], pVertices[i][1], pVertices[i][2]);
		}
	}
}

void CAnimation_Controller::ComputeSkinDeformation(FbxMesh* pMesh, FbxTime& fbxCurTime, FbxVector4* pVertices, _int iVertices)
{
	FbxSkin* pSkinDeformer = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType eSkinningType = pSkinDeformer->GetSkinningType();

	if ((eSkinningType == FbxSkin::eLinear) || (eSkinningType == FbxSkin::eRigid))
	{
		//ComputeLinear
	}
	else if (eSkinningType == FbxSkin::eDualQuaternion)
	{
		// ComputeQuaternion
	}
	else if (eSkinningType == FbxSkin::eBlend)
	{

	}
}
