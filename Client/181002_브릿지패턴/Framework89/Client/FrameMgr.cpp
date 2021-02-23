#include "stdafx.h"
#include "FrameMgr.h"

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
	: m_fDeltaTime(0.f), m_fSpf(0.f), m_fSpfTime(0.f), m_iFps(0), m_fFpsTime(0.f)
{
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}


CFrameMgr::~CFrameMgr()
{
}

void CFrameMgr::InitFrame(float framePersec)
{
	m_fSpf = 1.f / framePersec;

	// QueryPerformanceCounter: 하드웨어가 갖고 있는 고해상도 타이머 값을 얻어오는 함수. 1/10000초
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);

	// QueryPerformanceFrequency: CPU가 1초동안 진동하는 수를 얻어옴.
	QueryPerformanceFrequency(&m_CpuTick);
}

bool CFrameMgr::LockFrame()
{
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceFrequency(&m_CpuTick);

	m_fDeltaTime = float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;
	m_OldTime = m_CurTime;

	m_fSpfTime += m_fDeltaTime;

	if (m_fSpf <= m_fSpfTime)
	{		
		m_fSpfTime = 0.f;
		++m_iFps;
		return true;
	}

	return false;
}

void CFrameMgr::RenderFrame()
{
	m_fFpsTime += GET_INSTANCE(CTimeMgr)->GetDeltaTime();

	if (1.f <= m_fFpsTime)
	{
		swprintf_s(m_szFps, L"FPS: %d", m_iFps);
		m_iFps = 0;
		m_fFpsTime = 0.f;
	}

	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matTrans, 600.f, 100.f, 0.f);

	GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&matTrans);
	GET_INSTANCE(CDevice)->GetFont()->DrawTextW(GET_INSTANCE(CDevice)->GetSprite(), 
		m_szFps, lstrlen(m_szFps), nullptr, 0, D3DCOLOR_ARGB(255, 0, 255, 0));
}
