#include "framework.h"
#include "Timer.h"

CTimer::CTimer()
{
}

HRESULT CTimer::Ready_Timer()
{
	QueryPerformanceCounter(&m_frameTime);
	QueryPerformanceCounter(&m_fixTime);
	QueryPerformanceCounter(&m_lastTime);
	QueryPerformanceFrequency(&m_CPUTick);

	return S_OK;
}

const _float& CTimer::Get_TimeDelta()
{
	QueryPerformanceCounter(&m_frameTime);

	m_fTimeDelta = float(m_frameTime.QuadPart - m_lastTime.QuadPart) / m_CPUTick.QuadPart;

	m_lastTime = m_frameTime;

	if (m_frameTime.QuadPart - m_fixTime.QuadPart >= m_CPUTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CPUTick);
		m_fixTime = m_frameTime;
	}

	return m_fTimeDelta;
}

CTimer* CTimer::Create()
{
	CTimer* pInstance = new CTimer;

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
