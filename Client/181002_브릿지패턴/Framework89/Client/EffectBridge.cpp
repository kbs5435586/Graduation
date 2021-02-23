#include "stdafx.h"
#include "EffectBridge.h"


CEffectBridge::CEffectBridge()
	: m_bIsInit(false)
{
}


CEffectBridge::~CEffectBridge()
{
}

void CEffectBridge::LateInit()
{
	if (!m_bIsInit)
	{
		this->LateInit();
		m_bIsInit = true;
	}
}
