#pragma once
#include "Obj.h"

class CObserver;
class CUserInterface :
	public CObj
{
public:
	CUserInterface();
	virtual ~CUserInterface();

public:
	virtual HRESULT Initialize()	PURE;
	virtual void LateInit()			PURE;
	virtual int Update()			PURE;
	virtual void LateUpdate()		PURE;
	virtual void Render()			PURE;
	virtual void Release();

protected:
	CObserver*	m_pObserver;
};

