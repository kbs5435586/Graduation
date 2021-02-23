#pragma once
class CFrameMgr
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	CFrameMgr();
	~CFrameMgr();

public:
	void InitFrame(float framePersec);
	bool LockFrame();
	void RenderFrame();

private:
	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_CpuTick;

	float			m_fDeltaTime;
	float			m_fSpf;	// sec / frame
	float			m_fSpfTime;

	TCHAR			m_szFps[MIN_STR];
	int				m_iFps;
	float			m_fFpsTime;
};

