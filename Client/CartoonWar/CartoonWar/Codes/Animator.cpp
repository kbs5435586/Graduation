#include "framework.h"
#include "Animator.h"
#include "StructedBuffer.h"
#include "Mesh.h"
#include "Management.h"

CAnimator::CAnimator()
	: CComponent()
{
}

CAnimator::CAnimator(const CAnimator& rhs)
	: CComponent(rhs)
	, m_pBoneFinalMat(rhs.m_pBoneFinalMat)
{
}

HRESULT CAnimator::Ready_Animator()
{
	m_pBoneFinalMat = CStructedBuffer::Create();
	return S_OK;
}

void CAnimator::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());
}

void CAnimator::Update(const _float& fTimeDelta)
{
	m_fCurTime = 0.f;

	m_vecClipUpdateTime[m_iCurClip] += fTimeDelta;

	if (m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
	{
		m_vecClipUpdateTime[m_iCurClip] = 0.f;
	}

	m_fCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

	m_iFrameIdx = (int)(m_fCurTime * (float)m_iFrameCount);

	m_IsFinalMatUpdate = false;
}

void CAnimator::UpdateData(CMesh* pMesh)
{
	if (!m_IsFinalMatUpdate)
	{
		pMesh->GetBoneFrameData()->Update_Data_CS(TEXTURE_REGISTER::t10);
		pMesh->GetBoneOffset()->Update_Data_CS(TEXTURE_REGISTER::t11);
		
		CheckMesh(pMesh);
		m_pBoneFinalMat->Update_RWData(UAV_REGISTER::u0);

		UINT iBoneCount = (UINT)m_pVecBones->size();

		REP tRep;

		tRep.m_arrInt[0] = iBoneCount;
		tRep.m_arrInt[1] = m_iFrameIdx;

		
		int iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetUpContantBufferToShader_CS(CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);


		UINT iGrounX = (iBoneCount / 256) + 1;

		Dispatch(iGrounX, 1, 1);
		m_IsFinalMatUpdate = true;
	}

	// t12 레지스터에 최종행렬 데이터(구조버퍼) 바인딩
	m_pBoneFinalMat->Update_Data(TEXTURE_REGISTER::t12);
}

void CAnimator::Dispatch(int x, int y, int z)
{
	CDevice::GetInstance()->UpdateTable_CS();
	CDevice::GetInstance()->GetCsCmdLst()->Dispatch(x, y, z);
	CDevice::GetInstance()->ExcuteComputeShader();
}

void CAnimator::CheckMesh(CMesh* pMesh)
{
	_uint iBoneCnt = pMesh->GetBoneCount();
	if (m_pBoneFinalMat->GetElementCount() < iBoneCnt)
	{
		m_pBoneFinalMat->Ready_StructedBuffer(sizeof(Matrix), iBoneCnt, nullptr);
	}
}

CAnimator* CAnimator::Create()
{
	CAnimator* pInstance = new CAnimator();
	if (FAILED(pInstance->Ready_Animator()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* CAnimator::Clone_Component(void* pArg)
{
	return new CAnimator(*this);
}

void CAnimator::Free()
{
	Safe_Release(m_pBoneFinalMat);
	CComponent::Free();
}
