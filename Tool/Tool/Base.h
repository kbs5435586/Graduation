#pragma once
class CBase
{
protected:
	CBase();
	virtual ~CBase()=default;
public:
	_ulong		AddRef();
	_ulong		Release();
private:
	_ulong		m_dwRefCnt = 0;
protected:
	virtual void Free()PURE;
};

