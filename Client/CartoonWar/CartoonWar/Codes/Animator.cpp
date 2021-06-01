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
	, m_pBoneMat(rhs.m_pBoneMat)
{
	m_IsClone = true;
}

HRESULT CAnimator::Ready_Animator()
{

	return S_OK;
}

void CAnimator::LateInit()
{
	m_pBoneFinalMat = CStructedBuffer::Create();
	m_pBoneMat = CStructedBuffer::Create();
}

void CAnimator::SetBones(const vector<tMTBone>* _vecBones)
{
	m_pVecBones = _vecBones; 
	m_vecFinalBoneMat.resize(m_pVecBones->size());
}

void CAnimator::SetAnimClip(vector<tMTAnimClip> _vecAnimClip)
{
	m_vecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_vecClip.size());


	static float fTime = 0.f;
	fTime += 1.f;
	m_vecClipUpdateTime[0] = fTime;
}

_bool CAnimator::Update(AnimCtrl& tCtrl, const _float& fTimeDelta)
{
	_bool isRetVal = false;
	m_fCurTime = 0.f;

	tCtrl.fCurTime += fTimeDelta;

	if (tCtrl.fCurTime >= tCtrl.fEndTime - tCtrl.fStartTime)
	{                                 
		tCtrl.fCurTime = 0.f;
		isRetVal = true;
	}

	
	m_fCurTime = tCtrl.fStartTime + tCtrl.fCurTime;
	

	double dFrameIdx = m_fCurTime * m_iFrameCount;
	tCtrl.iCurFrm = (int)(dFrameIdx);

	if (tCtrl.iCurFrm >= tCtrl.fEndTime - 1)
		m_iNextFrameIdx = tCtrl.iCurFrm;
	else
		m_iNextFrameIdx = tCtrl.iCurFrm + 1;

	m_fRatio = (float)(dFrameIdx - (double)tCtrl.iCurFrm);
	m_iFrameIdx = (_uint)tCtrl.iCurFrm;
	m_IsFinalMatUpdate = false;
	return isRetVal;
}

void CAnimator::UpdateData(CMesh* pMesh, CShader* pShader)
{
	if (!m_IsFinalMatUpdate)
	{
		CDevice::GetInstance()->ClearDummyDesc_CS();

		pMesh->GetBoneFrameData()->Update_Data_CS(TEXTURE_REGISTER::t10);
		pMesh->GetBoneOffset()->Update_Data_CS(TEXTURE_REGISTER::t11);
		
		CheckMesh(pMesh);
		m_pBoneFinalMat->Update_RWData(UAV_REGISTER::u0);
		m_pBoneMat->Update_RWData(UAV_REGISTER::u1);

		UINT iBoneCount = (UINT)m_pVecBones->size();
		UINT iRow = 0;
		REP tRep;

		tRep.m_arrInt[0] = iBoneCount;
		tRep.m_arrInt[1] = m_iFrameIdx;
		tRep.m_arrInt[2] = m_iNextFrameIdx;
		tRep.m_arrInt[3] = iRow;
		tRep.m_arrFloat[0] = m_fRatio;
	
		
		_uint iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetUpContantBufferToShader_CS(
			CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);

		UINT iGrounX = (iBoneCount / 256) + 1;
		
		CDevice::GetInstance()->GetCsCmdLst()->SetPipelineState(pShader->GetCSPipeLine().Get());
		Dispatch(iGrounX, 1, 1);
		m_IsFinalMatUpdate = true;
	} 

	m_pBoneFinalMat->Update_Data(TEXTURE_REGISTER::t7);
	m_pBoneMat->Update_Data(TEXTURE_REGISTER::t8);
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
		m_pBoneFinalMat->Ready_StructedBuffer(sizeof(_matrix), iBoneCnt, nullptr);
		m_pBoneMat->Ready_StructedBuffer(sizeof(_matrix), iBoneCnt, nullptr);
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
	if(!m_IsClone)
		Safe_Release(m_pBoneFinalMat);
	CComponent::Free();
}
