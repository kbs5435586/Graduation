#pragma once
#include "Base.h"
class CFrame :
	public CBase
{
public:
	CFrame();
	virtual ~CFrame()=default;
public:
	HRESULT			Ready_Frame(const _float& fCallPerSec);
	_bool			Permit_Call(const _float& fTimeDelta);
public:
	static	CFrame*	Create(const _float& fCallPerSec);
	virtual void	Free();
private:
	_float			m_fTimeAcc = 0.f;
	_float			m_fInterval = 0.f;
};

