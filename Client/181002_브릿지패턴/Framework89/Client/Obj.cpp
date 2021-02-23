#include "stdafx.h"
#include "Obj.h"


CObj::CObj()
	: m_bIsInit(false)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));
	ZeroMemory(&m_tFrame, sizeof(FRAME));
}


CObj::~CObj()
{
}

void CObj::LateInit()
{
	if (!m_bIsInit)
	{
		this->LateInit();
		m_bIsInit = true;
	}
}

void CObj::MoveFrame()
{
	m_tFrame.fFrame += m_tFrame.fMax * GET_INSTANCE(CTimeMgr)->GetDeltaTime();

	if (m_tFrame.fMax < m_tFrame.fFrame)
		m_tFrame.fFrame = 0.f;
}
