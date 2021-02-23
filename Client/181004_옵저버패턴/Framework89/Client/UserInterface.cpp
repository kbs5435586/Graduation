#include "stdafx.h"
#include "UserInterface.h"
#include "Observer.h"

CUserInterface::CUserInterface()
	: m_pObserver(nullptr)
{
}


CUserInterface::~CUserInterface()
{
	Release();
}

void CUserInterface::Release()
{
	SafeDelete<CObserver*>(m_pObserver);
}

