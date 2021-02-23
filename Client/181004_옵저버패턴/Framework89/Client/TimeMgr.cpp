#include "stdafx.h"
#include "TimeMgr.h"

IMPLEMENT_SINGLETON(CTimeMgr)

CTimeMgr::CTimeMgr()
	: m_fDeltaTime(0.f)
{
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}


CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::InitTime()
{
	// QueryPerformanceCounter: �ϵ��� ���� �ִ� ���ػ� Ÿ�̸� ���� ������ �Լ�. 1/10000��
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);

	// QueryPerformanceFrequency: CPU�� 1�ʵ��� �����ϴ� ���� ����.
	QueryPerformanceFrequency(&m_CpuTick);
}

void CTimeMgr::UpdateTime()
{
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceFrequency(&m_CpuTick);

	m_fDeltaTime = float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;
	m_OldTime = m_CurTime;
}
