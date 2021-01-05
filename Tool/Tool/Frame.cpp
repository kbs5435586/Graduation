#include "stdafx.h"
#include "Frame.h"


CFrame::CFrame()
{
}

HRESULT CFrame::Ready_Frame(const _float & fCallPerSec)
{
	m_fInterval = 1.f / fCallPerSec;
	return S_OK;
}

_bool CFrame::Permit_Call(const _float & fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fInterval <= m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;
		return true;
	}

	return false;
}

CFrame * CFrame::Create(const _float & fCallPerSec)
{
	CFrame*	pInstance = new CFrame;

	if (FAILED(pInstance->Ready_Frame(fCallPerSec)))
		Safe_Release(pInstance);

	return pInstance;

}

void CFrame::Free()
{
}

