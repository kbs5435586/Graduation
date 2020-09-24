#include "framework.h"
#include "Animation_Controller.h"

CAnimation_Controller::CAnimation_Controller()
{
}

HRESULT CAnimation_Controller::Ready_Animation_Controller(FbxScene* pFbxScene)
{
	FbxArray<FbxString*>	FbxAnimationStackNames;
	pFbxScene->FillAnimStackNameArray(FbxAnimationStackNames);
	m_iAnimationStack = FbxAnimationStackNames.Size();

	m_ppFbxAnimationStacks = new FbxAnimStack*[m_iAnimationStack];
	m_pFbxStartTime = new FbxTime[m_iAnimationStack];
	m_pFbxStopTime = new FbxTime[m_iAnimationStack];
	m_pFbxCurrentTime = new FbxTime[m_iAnimationStack];


	for (_int i = 0; i < m_iAnimationStack; ++i)
	{
		FbxString* pFbxStackName = FbxAnimationStackNames[i];
		FbxAnimStack* pFbxAnimationStack = pFbxScene->FindMember<FbxAnimStack>(pFbxStackName->Buffer());
		m_ppFbxAnimationStacks[i] = pFbxAnimationStack;

		FbxTakeInfo* pFbxInfo = pFbxScene->GetTakeInfo(*pFbxStackName);
		FbxTime FbxStartTime, FbxStopTime;

		if (pFbxInfo)
		{
			FbxStartTime = pFbxInfo->mLocalTimeSpan.GetStart();
			FbxStopTime = pFbxInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan	FbxTimeLineTimeSpan;

			pFbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(FbxTimeLineTimeSpan);
			FbxStartTime = FbxTimeLineTimeSpan.GetStart();
			FbxStopTime = FbxTimeLineTimeSpan.GetStop();
		}


		m_pFbxStartTime[i] = FbxStartTime;
		m_pFbxStopTime[i] = FbxStopTime;
		m_pFbxCurrentTime[i] = FbxTime(0);
	}

	FbxArrayDelete(FbxAnimationStackNames);
	return S_OK;
}

HRESULT CAnimation_Controller::Set_Animation(FbxScene* pFbxScene, const _uint& iAnimation_Num)
{
	if (nullptr == pFbxScene)
		return E_FAIL;

	m_iAnimationStack = iAnimation_Num;
	pFbxScene->SetCurrentAnimationStack(m_ppFbxAnimationStacks[m_iAnimationStack]);
	return S_OK;
}

void CAnimation_Controller::Play_Animation(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;
	FbxTime  FbxEllapseTime;
	FbxEllapseTime.SetSecondDouble(fTimeDelta);

	m_pFbxCurrentTime[m_iAnimationStack] += FbxEllapseTime;
	if (m_pFbxCurrentTime[m_iAnimationStack] > m_pFbxStopTime[m_iAnimationStack])
		m_pFbxCurrentTime[m_iAnimationStack] = m_pFbxStopTime[m_iAnimationStack];
}

CAnimation_Controller* CAnimation_Controller::Create(FbxScene* pFbxScene)
{
	CAnimation_Controller* pInstance = new CAnimation_Controller();
	if (FAILED(pInstance->Ready_Animation_Controller(pFbxScene)))
	{
		MessageBox(0, L"CAnimation_Controller Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation_Controller::Free()
{
	if (m_ppFbxAnimationStacks) delete[] m_ppFbxAnimationStacks;
	if (m_pFbxStartTime) delete[] m_pFbxStartTime;
	if (m_pFbxStopTime) delete[] m_pFbxStopTime;
	if (m_pFbxCurrentTime) delete[] m_pFbxCurrentTime;
}

void CAnimation_Controller::Set_Position(_uint iAnimationStack, _float fPosition)
{
	m_pFbxCurrentTime[iAnimationStack].SetSecondDouble(fPosition);
}
