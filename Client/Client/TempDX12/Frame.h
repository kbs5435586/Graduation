#pragma once
#include "Base.h"
class CFrame :
    public CBase
{
private:
    CFrame();
	virtual ~CFrame() = default;
public:
	HRESULT			Ready_Frame(const _float& fCallPerSec);
	bool			Permit_Call(const _float& fTimeDelta);
private:
	_float			m_fTimeAcc = 0.f;
	_float			m_fInterval = 0.f;
public:
	static CFrame*	Create(const _float& fCallPerSec);
	virtual void	Free(void);
};

