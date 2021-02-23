#pragma once
#include "Effect.h"
class CBuffEffect :
	public CEffect
{
public:
	CBuffEffect();
	virtual ~CBuffEffect();

public:
	virtual HRESULT Initialize();
	virtual void LateInit();
	virtual int Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Release();

private:
	CObj*	m_pTarget;
	float	m_fRevAngle;
};

