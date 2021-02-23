#pragma once

#include "Obj.h"

class CEffectBridge;
class CEffect : public CObj	// Ãß»óÃþ
{
public:
	CEffect();
	virtual ~CEffect();

public:
	void SetBridge(CEffectBridge* pBridge) { m_pBridge = pBridge; }

public:
	virtual HRESULT Initialize();
	virtual void LateInit();
	virtual int Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Release();

protected:
	CEffectBridge*	m_pBridge;
};

