#include "framework.h"
#include "Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	return ++m_dwRefCnt;
}

_ulong CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		if(this)
			delete this;

		return 0;
	}

	else
		return m_dwRefCnt--;
}
