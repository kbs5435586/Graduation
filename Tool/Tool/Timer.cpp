#include "stdafx.h"
#include "Timer.h"


CTimer::CTimer()
{

}

HRESULT CTimer::Ready_Timer()
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

const _float & CTimer::Get_TimeDelta()
{
	QueryPerformanceCounter(&m_FrameTime);

	m_fTimeDelta = float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / m_CpuTick.QuadPart;

	m_LastTime = m_FrameTime;

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	return m_fTimeDelta;
}

CTimer * CTimer::Create()
{
	CTimer*	pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		_MSG_BOX("CTimer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}

