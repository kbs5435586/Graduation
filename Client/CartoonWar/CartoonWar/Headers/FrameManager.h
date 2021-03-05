#pragma once
#include "Base.h"
class CFrame;
class CFrameManager :
    public CBase
{
	_DECLARE_SINGLETON(CFrameManager);
public:
    CFrameManager();
    virtual ~CFrameManager() = default;
public:
	HRESULT								Add_Frame(const _tchar* pFrameTag, const _float& fCallCnt);
	_bool								Permit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
private:
	CFrame*								Find_Frame(const _tchar* pFrameTag);
	virtual void						Free();
private:
	map<const _tchar*, CFrame*>			m_mapFrame;
	typedef map<const _tchar*, CFrame*>	MAPFRAME;

};

